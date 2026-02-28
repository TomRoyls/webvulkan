#include "spirv_parser.h"
#include <stdlib.h>
#include <string.h>
#include "../util/log.h"

static uint32_t read_word(WgvkSpvModule *module) {
	if (module->cursor >= module->word_count)
		return 0;
	return module->words[module->cursor++];
}

static const char *read_string(WgvkSpvModule *module, size_t *out_len) {
	size_t start = module->string_cursor;
	char *dest = module->string_buffer + start;
	size_t max_len = sizeof(module->string_buffer) - start;
	if (max_len == 0) {
		if (out_len)
			*out_len = 0;
		dest[0] = '\0';
		return dest;
	}

	size_t len = 0;
	while (module->cursor < module->word_count) {
		uint32_t word = module->words[module->cursor];
		module->cursor++;

		for (int i = 0; i < 4; i++) {
			char c = (char)((word >> (i * 8)) & 0xFF);
			if (c == '\0') {
				dest[len] = '\0';
				if (out_len)
					*out_len = len;
				module->string_cursor += len + 1;
				return dest;
			}
			if (len + 1 < max_len) {
				dest[len++] = c;
			}
		}
	}

	dest[len < max_len ? len : max_len - 1] = '\0';
	if (out_len)
		*out_len = len;
	module->string_cursor += len + 1;
	return dest;
}

static void skip_instruction(WgvkSpvModule *module, uint16_t word_count) {
	module->cursor += word_count - 1;
}

/* Emit a one-shot warning when a fixed-size table overflows. */
static void warn_table_full_once(int *warned, const char *table_name, int capacity) {
	if (!*warned) {
		*warned = 1;
		WGVK_WARN(WGVK_LOG_CAT_SHADER,
		          "SPIR-V %s table full (%d entries); "
		          "additional entries will be silently ignored",
		          table_name, capacity);
	}
}

static void add_instruction_to_block(WgvkSpvModule *module, uint32_t opcode, uint32_t result_id,
                                     uint32_t result_type_id, uint32_t *operands,
                                     uint32_t operand_count) {
	if (!module->current_function)
		return;

	WgvkSpvFunction *func = module->current_function;
	if (func->block_count == 0)
		return;
	WgvkSpvBlock *block = &func->blocks[func->current_block];
	if (block->instruction_count >= 64)
		return;

	WgvkSpvInstruction *inst = &block->instructions[block->instruction_count++];
	inst->opcode = opcode;
	inst->result_id = result_id;
	inst->result_type_id = result_type_id;
	inst->operand_count = operand_count < 16 ? operand_count : 16;
	for (uint32_t i = 0; i < inst->operand_count; i++) {
		inst->operands[i] = operands[i];
	}
}

int wgvk_spirv_parse(WgvkSpvModule *module, const uint32_t *code, size_t word_count) {
	if (!module || !code || word_count < 5)
		return -1;

	memset(module, 0, sizeof(*module));
	module->words = code;
	module->word_count = word_count;
	module->cursor = 0;

	/* One-shot overflow warning flags — fired at most once per table per parse. */
	int warned_types = 0;
	int warned_constants = 0;
	int warned_variables = 0;
	int warned_decorations = 0;

	uint32_t magic = read_word(module);
	if (magic != WGVK_SPV_MAGIC)
		return -1;

	read_word(module);
	read_word(module);
	read_word(module);
	read_word(module);

	while (module->cursor < module->word_count) {
		size_t inst_start = module->cursor;
		uint32_t word = read_word(module);
		uint16_t opcode = word & 0xFFFF;
		uint16_t word_count = (word >> 16) & 0xFFFF;

		if (word_count == 0)
			word_count = 1;

		switch (opcode) {
		case WGVK_SPV_OP_CAPABILITY:
		case WGVK_SPV_OP_EXTENSION:
		case WGVK_SPV_OP_EXT_INST_IMPORT:
		case WGVK_SPV_OP_MEMORY_MODEL:
		case WGVK_SPV_OP_SOURCE:
		case WGVK_SPV_OP_NAME:
		case WGVK_SPV_OP_MEMBER_NAME:
		case WGVK_SPV_OP_DECORATION_GROUP:
		case WGVK_SPV_OP_GROUP_DECORATE:
		case WGVK_SPV_OP_GROUP_MEMBER_DECORATE:
			skip_instruction(module, word_count);
			break;

		case WGVK_SPV_OP_EXECUTION_MODE: {
			/* OpExecutionMode: entry_point_id, mode, optional operands */
			if (word_count >= 3) {
				uint32_t ep_id = read_word(module);
				uint32_t mode  = read_word(module);
				/* Mode 17 = LocalSize (compute workgroup dimensions) */
				if (mode == 17 && word_count >= 6) {
					uint32_t sx = read_word(module);
					uint32_t sy = read_word(module);
					uint32_t sz = read_word(module);
					for (uint32_t i = 0; i < module->entry_point_count; i++) {
						if (module->entry_points[i].entry_point_id == ep_id) {
							module->entry_points[i].local_size_x = sx;
							module->entry_points[i].local_size_y = sy;
							module->entry_points[i].local_size_z = sz;
							break;
						}
					}
				} else {
					/* Skip remaining operands for other modes */
					size_t consumed = module->cursor - inst_start;
					if (consumed < word_count)
						module->cursor += word_count - consumed;
				}
			} else {
				skip_instruction(module, word_count);
			}
			break;
		}

		case WGVK_SPV_OP_ENTRY_POINT: {
			if (module->entry_point_count < WGVK_MAX_ENTRY_POINTS) {
				WgvkSpvEntryPoint *ep = &module->entry_points[module->entry_point_count++];
				ep->exec_model = read_word(module);
				ep->entry_point_id = read_word(module);
				ep->name = read_string(module, NULL);

				size_t remaining = word_count - (module->cursor - inst_start);
				ep->interface_count = remaining;
				for (size_t i = 0; i < remaining && i < 32; i++) {
					ep->interface_ids[i] = read_word(module);
				}
			} else {
				WGVK_WARN(WGVK_LOG_CAT_SHADER,
				          "SPIR-V entry point table full (%d entries); "
				          "additional entry points will be ignored",
				          WGVK_MAX_ENTRY_POINTS);
				skip_instruction(module, word_count);
			}
			break;
		}

		case WGVK_SPV_OP_DECORATE: {
			if (module->decoration_count < WGVK_MAX_DECORATIONS && word_count >= 3) {
				WgvkSpvDecorationInfo *d = &module->decorations[module->decoration_count++];
				d->target_id = read_word(module);
				d->decoration = read_word(module);
				d->member = 0xFFFFFFFF;
				d->value = (word_count >= 4) ? read_word(module) : 0;
			} else if (module->decoration_count >= WGVK_MAX_DECORATIONS) {
				warn_table_full_once(&warned_decorations, "decoration", WGVK_MAX_DECORATIONS);
				skip_instruction(module, word_count);
			} else {
				skip_instruction(module, word_count);
			}
			break;
		}

		case WGVK_SPV_OP_MEMBER_DECORATE: {
			if (module->decoration_count < WGVK_MAX_DECORATIONS && word_count >= 4) {
				WgvkSpvDecorationInfo *d = &module->decorations[module->decoration_count++];
				d->target_id = read_word(module);
				d->member = read_word(module);
				d->decoration = read_word(module);
				d->value = (word_count >= 5) ? read_word(module) : 0;
			} else if (module->decoration_count >= WGVK_MAX_DECORATIONS) {
				warn_table_full_once(&warned_decorations, "decoration", WGVK_MAX_DECORATIONS);
				skip_instruction(module, word_count);
			} else {
				skip_instruction(module, word_count);
			}
			break;
		}

		case WGVK_SPV_OP_TYPE_VOID:
		case WGVK_SPV_OP_TYPE_BOOL: {
			if (module->type_count < WGVK_MAX_TYPES && word_count >= 2) {
				WgvkSpvType *t = &module->types[module->type_count++];
				t->id = read_word(module);
				t->op = opcode;
			} else {
				if (module->type_count >= WGVK_MAX_TYPES)
					warn_table_full_once(&warned_types, "type", WGVK_MAX_TYPES);
				skip_instruction(module, word_count);
			}
			break;
		}

		case WGVK_SPV_OP_TYPE_INT: {
			if (module->type_count < WGVK_MAX_TYPES && word_count >= 4) {
				WgvkSpvType *t = &module->types[module->type_count++];
				t->id = read_word(module);
				t->op = opcode;
				t->width = read_word(module);
				t->signedness = read_word(module);
			} else {
				if (module->type_count >= WGVK_MAX_TYPES)
					warn_table_full_once(&warned_types, "type", WGVK_MAX_TYPES);
				skip_instruction(module, word_count);
			}
			break;
		}

		case WGVK_SPV_OP_TYPE_FLOAT: {
			if (module->type_count < WGVK_MAX_TYPES && word_count >= 3) {
				WgvkSpvType *t = &module->types[module->type_count++];
				t->id = read_word(module);
				t->op = opcode;
				t->width = read_word(module);
			} else {
				if (module->type_count >= WGVK_MAX_TYPES)
					warn_table_full_once(&warned_types, "type", WGVK_MAX_TYPES);
				skip_instruction(module, word_count);
			}
			break;
		}

		case WGVK_SPV_OP_TYPE_VECTOR: {
			if (module->type_count < WGVK_MAX_TYPES && word_count >= 4) {
				WgvkSpvType *t = &module->types[module->type_count++];
				t->id = read_word(module);
				t->op = opcode;
				t->element_type = read_word(module);
				t->vector_size = read_word(module);
			} else {
				if (module->type_count >= WGVK_MAX_TYPES)
					warn_table_full_once(&warned_types, "type", WGVK_MAX_TYPES);
				skip_instruction(module, word_count);
			}
			break;
		}

		case WGVK_SPV_OP_TYPE_MATRIX: {
			if (module->type_count < WGVK_MAX_TYPES && word_count >= 4) {
				WgvkSpvType *t = &module->types[module->type_count++];
				t->id = read_word(module);
				t->op = opcode;
				t->element_type = read_word(module);
				t->matrix_cols = read_word(module);
			} else {
				if (module->type_count >= WGVK_MAX_TYPES)
					warn_table_full_once(&warned_types, "type", WGVK_MAX_TYPES);
				skip_instruction(module, word_count);
			}
			break;
		}

		case WGVK_SPV_OP_TYPE_ARRAY: {
			if (module->type_count < WGVK_MAX_TYPES && word_count >= 4) {
				WgvkSpvType *t = &module->types[module->type_count++];
				t->id = read_word(module);
				t->op = opcode;
				t->element_type = read_word(module);
				t->length = read_word(module);
			} else {
				if (module->type_count >= WGVK_MAX_TYPES)
					warn_table_full_once(&warned_types, "type", WGVK_MAX_TYPES);
				skip_instruction(module, word_count);
			}
			break;
		}

		case WGVK_SPV_OP_TYPE_STRUCT: {
			if (module->type_count < WGVK_MAX_TYPES && word_count >= 2) {
				WgvkSpvType *t = &module->types[module->type_count++];
				t->id = read_word(module);
				t->op = opcode;
				t->member_count = word_count - 2;
				for (uint32_t i = 0; i < t->member_count && i < 16; i++) {
					t->member_types[i] = read_word(module);
				}
			} else {
				if (module->type_count >= WGVK_MAX_TYPES)
					warn_table_full_once(&warned_types, "type", WGVK_MAX_TYPES);
				skip_instruction(module, word_count);
			}
			break;
		}

		case WGVK_SPV_OP_TYPE_POINTER: {
			if (module->type_count < WGVK_MAX_TYPES && word_count >= 4) {
				WgvkSpvType *t = &module->types[module->type_count++];
				t->id = read_word(module);
				t->op = opcode;
				t->storage_class = read_word(module);
				t->element_type = read_word(module);
			} else {
				if (module->type_count >= WGVK_MAX_TYPES)
					warn_table_full_once(&warned_types, "type", WGVK_MAX_TYPES);
				skip_instruction(module, word_count);
			}
			break;
		}

		case WGVK_SPV_OP_TYPE_FUNCTION: {
			if (module->type_count < WGVK_MAX_TYPES && word_count >= 3) {
				WgvkSpvType *t = &module->types[module->type_count++];
				t->id = read_word(module);
				t->op = opcode;
				t->return_type = read_word(module);
				t->param_count = word_count - 3;
				for (uint32_t i = 0; i < t->param_count && i < 8; i++) {
					t->param_types[i] = read_word(module);
				}
			} else {
				if (module->type_count >= WGVK_MAX_TYPES)
					warn_table_full_once(&warned_types, "type", WGVK_MAX_TYPES);
				skip_instruction(module, word_count);
			}
			break;
		}

		case WGVK_SPV_OP_CONSTANT_TRUE:
		case WGVK_SPV_OP_CONSTANT_FALSE: {
			if (module->constant_count < WGVK_MAX_CONSTANTS && word_count >= 3) {
				WgvkSpvConstant *c = &module->constants[module->constant_count++];
				c->type_id = read_word(module);
				c->id = read_word(module);
				c->value[0] = (opcode == WGVK_SPV_OP_CONSTANT_TRUE) ? 1 : 0;
			} else {
				if (module->constant_count >= WGVK_MAX_CONSTANTS)
					warn_table_full_once(&warned_constants, "constant", WGVK_MAX_CONSTANTS);
				skip_instruction(module, word_count);
			}
			break;
		}

		case WGVK_SPV_OP_CONSTANT: {
			if (module->constant_count < WGVK_MAX_CONSTANTS && word_count >= 4) {
				WgvkSpvConstant *c = &module->constants[module->constant_count++];
				c->type_id = read_word(module);
				c->id = read_word(module);

				size_t value_words = word_count - 3;
				for (size_t i = 0; i < value_words && i < 4; i++) {
					c->value[i] = read_word(module);
				}

				if (value_words == 1) {
					memcpy(&c->fvalue[0], &c->value[0], sizeof(float));
				} else if (value_words == 2) {
					uint64_t bits = ((uint64_t)c->value[1] << 32) | c->value[0];
					double d;
					memcpy(&d, &bits, sizeof(double));
					c->fvalue[0] = (float)d;
				}
			} else {
				if (module->constant_count >= WGVK_MAX_CONSTANTS)
					warn_table_full_once(&warned_constants, "constant", WGVK_MAX_CONSTANTS);
				skip_instruction(module, word_count);
			}
			break;
		}

		case WGVK_SPV_OP_CONSTANT_COMPOSITE: {
			if (module->constant_count < WGVK_MAX_CONSTANTS && word_count >= 4) {
				WgvkSpvConstant *c = &module->constants[module->constant_count++];
				c->type_id = read_word(module);
				c->id = read_word(module);
				c->component_count = word_count - 3;
				for (uint32_t i = 0; i < c->component_count && i < 16; i++) {
					c->component_ids[i] = read_word(module);
				}
			} else {
				if (module->constant_count >= WGVK_MAX_CONSTANTS)
					warn_table_full_once(&warned_constants, "constant", WGVK_MAX_CONSTANTS);
				skip_instruction(module, word_count);
			}
			break;
		}

		case WGVK_SPV_OP_VARIABLE: {
			if (module->variable_count < WGVK_MAX_VARIABLES && word_count >= 4) {
				WgvkSpvVariable *v = &module->variables[module->variable_count++];
				v->type_id = read_word(module);
				v->id = read_word(module);
				v->storage_class = read_word(module);
			} else {
				if (module->variable_count >= WGVK_MAX_VARIABLES)
					warn_table_full_once(&warned_variables, "variable", WGVK_MAX_VARIABLES);
				skip_instruction(module, word_count);
			}
			break;
		}

		case WGVK_SPV_OP_FUNCTION: {
			if (word_count >= 5) {
				if (module->function_count >= module->function_capacity) {
					uint32_t new_cap =
					    module->function_capacity ? module->function_capacity * 2 : 8;
					WgvkSpvFunction **new_funcs =
					    realloc(module->functions, new_cap * sizeof(WgvkSpvFunction *));
					if (!new_funcs) {
						skip_instruction(module, word_count);
						break;
					}
					module->functions = new_funcs;
					module->function_capacity = new_cap;
				}
				WgvkSpvFunction *f = calloc(1, sizeof(WgvkSpvFunction));
				if (!f) {
					skip_instruction(module, word_count);
					break;
				}
				f->result_type_id = read_word(module);
				f->id = read_word(module);
				read_word(module);
				read_word(module);
				f->blocks = calloc(8, sizeof(WgvkSpvBlock));
				if (!f->blocks) {
					free(f);
					skip_instruction(module, word_count);
					break;
				}
				f->block_capacity = 8;
				f->block_count = 0;
				f->current_block = 0;
				module->functions[module->function_count++] = f;
				module->current_function = f;
			} else {
				skip_instruction(module, word_count);
			}
			break;
		}

		case WGVK_SPV_OP_FUNCTION_END: {
			module->current_function = NULL;
			break;
		}

		case WGVK_SPV_OP_LABEL: {
			if (module->current_function) {
				WgvkSpvFunction *func = module->current_function;
				if (func->block_count >= func->block_capacity) {
					uint32_t new_cap = func->block_capacity * 2;
					WgvkSpvBlock *new_blocks =
					    realloc(func->blocks, new_cap * sizeof(WgvkSpvBlock));
					if (!new_blocks) {
						skip_instruction(module, word_count);
						break;
					}
					/* Zero-init the new blocks */
					memset(new_blocks + func->block_capacity, 0,
					       (new_cap - func->block_capacity) * sizeof(WgvkSpvBlock));
					func->blocks = new_blocks;
					func->block_capacity = new_cap;
				}
				uint32_t label_id = read_word(module);
				WgvkSpvBlock *block = &func->blocks[func->block_count++];
				block->label_id = label_id;
				block->instruction_count = 0;
				func->current_block = func->block_count - 1;
			} else {
				skip_instruction(module, word_count);
			}
			break;
		}

		case WGVK_SPV_OP_RETURN:
		case WGVK_SPV_OP_RETURN_VALUE:
		case WGVK_SPV_OP_BRANCH:
		case WGVK_SPV_OP_BRANCH_CONDITIONAL:
		case WGVK_SPV_OP_KILL: {
			uint32_t operands[16] = {0};
			uint32_t operand_count = word_count - 1;
			for (uint32_t i = 0; i < operand_count && i < 16; i++) {
				operands[i] = read_word(module);
			}
			add_instruction_to_block(module, opcode, 0, 0, operands, operand_count);
			break;
		}

		case WGVK_SPV_OP_FADD:
		case WGVK_SPV_OP_FSUB:
		case WGVK_SPV_OP_FMUL:
		case WGVK_SPV_OP_FDIV:
		case WGVK_SPV_OP_IADD:
		case WGVK_SPV_OP_ISUB:
		case WGVK_SPV_OP_IMUL:
		case WGVK_SPV_OP_SDIV:
		case WGVK_SPV_OP_UDIV:
		case WGVK_SPV_OP_SNEGATE:
		case WGVK_SPV_OP_FNEGATE:
		case WGVK_SPV_OP_DOT:
		case WGVK_SPV_OP_VECTOR_TIMES_SCALAR:
		case WGVK_SPV_OP_MATRIX_TIMES_VECTOR:
		case WGVK_SPV_OP_VECTOR_TIMES_MATRIX:
		case WGVK_SPV_OP_MATRIX_TIMES_MATRIX:
		case WGVK_SPV_OP_OUTER_PRODUCT:
		case WGVK_SPV_OP_TRANSPOSE:
		case WGVK_SPV_OP_LOGICAL_AND:
		case WGVK_SPV_OP_LOGICAL_OR:
		case WGVK_SPV_OP_LOGICAL_NOT:
		case WGVK_SPV_OP_LOGICAL_EQUAL:
		case WGVK_SPV_OP_LOGICAL_NOT_EQUAL:
		case WGVK_SPV_OP_F_ORD_EQUAL:
		case WGVK_SPV_OP_F_ORD_NOT_EQUAL:
		case WGVK_SPV_OP_F_ORD_LESS_THAN:
		case WGVK_SPV_OP_F_ORD_GREATER_THAN:
		case WGVK_SPV_OP_F_ORD_LESS_THAN_EQUAL:
		case WGVK_SPV_OP_F_ORD_GREATER_THAN_EQUAL:
		case WGVK_SPV_OP_CONVERT_S_TO_F:
		case WGVK_SPV_OP_CONVERT_U_TO_F:
		case WGVK_SPV_OP_CONVERT_F_TO_S:
		case WGVK_SPV_OP_CONVERT_F_TO_U:
		case WGVK_SPV_OP_S_CONVERT:
		case WGVK_SPV_OP_U_CONVERT:
		case WGVK_SPV_OP_F_CONVERT:
		case WGVK_SPV_OP_BITCAST:
		case WGVK_SPV_OP_SELECT:
		case WGVK_SPV_OP_ISNAN:
		case WGVK_SPV_OP_ISINF:
		case WGVK_SPV_OP_BITWISE_OR:
		case WGVK_SPV_OP_BITWISE_XOR:
		case WGVK_SPV_OP_BITWISE_AND:
		case WGVK_SPV_OP_NOT:
		case WGVK_SPV_OP_SHIFT_LEFT_LOGICAL:
		case WGVK_SPV_OP_SHIFT_RIGHT_LOGICAL:
		case WGVK_SPV_OP_SHIFT_RIGHT_ARITHMETIC: {
			if (word_count >= 4) {
				uint32_t result_type = read_word(module);
				uint32_t result_id = read_word(module);
				uint32_t operands[16] = {0};
				uint32_t operand_count = word_count - 4;
				for (uint32_t i = 0; i < operand_count && i < 16; i++) {
					operands[i] = read_word(module);
				}
				add_instruction_to_block(module, opcode, result_id, result_type, operands,
				                         operand_count);
			} else {
				skip_instruction(module, word_count);
			}
			break;
		}

		case WGVK_SPV_OP_LOAD: {
			if (word_count >= 4) {
				uint32_t result_type = read_word(module);
				uint32_t result_id = read_word(module);
				uint32_t operands[16] = {0};
				uint32_t operand_count = word_count - 3;
				for (uint32_t i = 0; i < operand_count && i < 16; i++) {
					operands[i] = read_word(module);
				}
				add_instruction_to_block(module, opcode, result_id, result_type, operands,
				                         operand_count);
			} else {
				skip_instruction(module, word_count);
			}
			break;
		}

		case WGVK_SPV_OP_STORE: {
			if (word_count >= 3) {
				uint32_t operands[16] = {0};
				uint32_t operand_count = word_count - 1;
				for (uint32_t i = 0; i < operand_count && i < 16; i++) {
					operands[i] = read_word(module);
				}
				add_instruction_to_block(module, opcode, 0, 0, operands, operand_count);
			} else {
				skip_instruction(module, word_count);
			}
			break;
		}

		case WGVK_SPV_OP_ACCESS_CHAIN:
		case WGVK_SPV_OP_IN_BOUNDS_ACCESS_CHAIN: {
			if (word_count >= 4) {
				uint32_t result_type = read_word(module);
				uint32_t result_id = read_word(module);
				uint32_t operands[16] = {0};
				uint32_t operand_count = word_count - 3;
				for (uint32_t i = 0; i < operand_count && i < 16; i++) {
					operands[i] = read_word(module);
				}
				add_instruction_to_block(module, opcode, result_id, result_type, operands,
				                         operand_count);
			} else {
				skip_instruction(module, word_count);
			}
			break;
		}

		case WGVK_SPV_OP_COMPOSITE_EXTRACT:
		case WGVK_SPV_OP_COMPOSITE_INSERT:
		case WGVK_SPV_OP_VECTOR_SHUFFLE:
		case WGVK_SPV_OP_COPY_OBJECT: {
			if (word_count >= 4) {
				uint32_t result_type = read_word(module);
				uint32_t result_id = read_word(module);
				uint32_t operands[16] = {0};
				uint32_t operand_count = word_count - 3;
				for (uint32_t i = 0; i < operand_count && i < 16; i++) {
					operands[i] = read_word(module);
				}
				add_instruction_to_block(module, opcode, result_id, result_type, operands,
				                         operand_count);
			} else {
				skip_instruction(module, word_count);
			}
			break;
		}

		case WGVK_SPV_OP_IMAGE_SAMPLE_IMPLICIT_LOD:
		case WGVK_SPV_OP_IMAGE: {
			if (word_count >= 4) {
				uint32_t result_type = read_word(module);
				uint32_t result_id = read_word(module);
				uint32_t operands[16] = {0};
				uint32_t operand_count = word_count - 3;
				for (uint32_t i = 0; i < operand_count && i < 16; i++) {
					operands[i] = read_word(module);
				}
				add_instruction_to_block(module, opcode, result_id, result_type, operands,
				                         operand_count);
			} else {
				skip_instruction(module, word_count);
			}
			break;
		}

		default:
			skip_instruction(module, word_count);
			break;
		}
	}

	return 0;
}

void wgvk_spirv_free(WgvkSpvModule *module) {
	if (!module)
		return;
	for (uint32_t i = 0; i < module->function_count; i++) {
		if (module->functions[i]) {
			free(module->functions[i]->blocks);
			free(module->functions[i]);
		}
	}
	free(module->functions);
	module->functions = NULL;
	module->function_count = 0;
	module->function_capacity = 0;
	module->current_function = NULL;
}

WgvkSpvType *wgvk_spirv_get_type(WgvkSpvModule *module, uint32_t id) {
	for (uint32_t i = 0; i < module->type_count; i++) {
		if (module->types[i].id == id) {
			return &module->types[i];
		}
	}
	return NULL;
}

WgvkSpvConstant *wgvk_spirv_get_constant(WgvkSpvModule *module, uint32_t id) {
	for (uint32_t i = 0; i < module->constant_count; i++) {
		if (module->constants[i].id == id) {
			return &module->constants[i];
		}
	}
	return NULL;
}

WgvkSpvVariable *wgvk_spirv_get_variable(WgvkSpvModule *module, uint32_t id) {
	for (uint32_t i = 0; i < module->variable_count; i++) {
		if (module->variables[i].id == id) {
			return &module->variables[i];
		}
	}
	return NULL;
}

WgvkSpvDecorationInfo *wgvk_spirv_get_decoration(WgvkSpvModule *module, uint32_t id,
                                                 uint32_t decoration) {
	for (uint32_t i = 0; i < module->decoration_count; i++) {
		if (module->decorations[i].target_id == id &&
		    module->decorations[i].decoration == decoration) {
			return &module->decorations[i];
		}
	}
	return NULL;
}

const char *wgvk_spirv_get_entry_name(WgvkSpvModule *module, uint32_t exec_model) {
	for (uint32_t i = 0; i < module->entry_point_count; i++) {
		if (module->entry_points[i].exec_model == exec_model) {
			return module->entry_points[i].name;
		}
	}
	return "main";
}

WgvkSpvFunction *wgvk_spirv_get_function(WgvkSpvModule *module, uint32_t id) {
	for (uint32_t i = 0; i < module->function_count; i++) {
		if (module->functions[i] && module->functions[i]->id == id) {
			return module->functions[i];
		}
	}
	return NULL;
}
