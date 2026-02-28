#include "wgsl_gen.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../util/log.h"

static int emit(WgvkWgslGenerator *gen, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	/* Measure how many bytes are needed */
	va_list args2;
	va_copy(args2, args);
	int needed = vsnprintf(NULL, 0, fmt, args2);
	va_end(args2);
	va_end(args);
	if (needed <= 0)
		return 0;

	size_t required = gen->cursor + (size_t)needed + 1;
	if (required > gen->capacity) {
		size_t new_cap = gen->capacity ? gen->capacity * 2 : 65536;
		while (new_cap < required)
			new_cap *= 2;
		char *new_buf = realloc(gen->buffer, new_cap);
		if (!new_buf)
			return -1;
		gen->buffer = new_buf;
		gen->capacity = new_cap;
	}

	va_list args3;
	va_start(args3, fmt);
	int written = vsnprintf(gen->buffer + gen->cursor, gen->capacity - gen->cursor, fmt, args3);
	va_end(args3);
	if (written > 0)
		gen->cursor += (size_t)written;
	return written;
}

static void emit_type(WgvkWgslGenerator *gen, WgvkSpvType *type);

static void emit_type(WgvkWgslGenerator *gen, WgvkSpvType *type) {
	if (!type) {
		emit(gen, "f32");
		return;
	}

	switch (type->op) {
	case WGVK_SPV_OP_TYPE_VOID:
		break;
	case WGVK_SPV_OP_TYPE_BOOL:
		emit(gen, "bool");
		break;
	case WGVK_SPV_OP_TYPE_INT:
		if (type->width == 32) {
			emit(gen, type->signedness ? "i32" : "u32");
		} else if (type->width == 64) {
			WGVK_WARN(WGVK_LOG_CAT_SHADER,
			          "SPIR-V type id=%u: 64-bit integer downcast to 32-bit in WGSL "
			          "(WGSL does not support 64-bit integers)",
			          type->id);
			emit(gen, type->signedness ? "i32" : "u32");
		} else {
			emit(gen, "i32");
		}
		break;
	case WGVK_SPV_OP_TYPE_FLOAT:
		if (type->width == 32) {
			emit(gen, "f32");
		} else if (type->width == 64) {
			WGVK_WARN(WGVK_LOG_CAT_SHADER,
			          "SPIR-V type id=%u: 64-bit float (double) downcast to f32 in WGSL "
			          "(WGSL does not support 64-bit floats)",
			          type->id);
			emit(gen, "f32");
		} else {
			emit(gen, "f32");
		}
		break;
	case WGVK_SPV_OP_TYPE_VECTOR: {
		WgvkSpvType *elem = wgvk_spirv_get_type(gen->module, type->element_type);
		if (elem && elem->op == WGVK_SPV_OP_TYPE_FLOAT) {
			emit(gen, "vec%u<f32>", type->vector_size);
		} else if (elem && elem->op == WGVK_SPV_OP_TYPE_INT) {
			emit(gen, "vec%u<%s>", type->vector_size, elem->signedness ? "i32" : "u32");
		} else {
			emit(gen, "vec%u<f32>", type->vector_size);
		}
		break;
	}
	case WGVK_SPV_OP_TYPE_MATRIX: {
		WgvkSpvType *elem = wgvk_spirv_get_type(gen->module, type->element_type);
		if (elem && elem->op == WGVK_SPV_OP_TYPE_VECTOR) {
			emit(gen, "mat%ux%u<f32>", type->matrix_cols, elem->vector_size);
		} else {
			emit(gen, "mat4x4<f32>");
		}
		break;
	}
	case WGVK_SPV_OP_TYPE_ARRAY: {
		WgvkSpvType *elem = wgvk_spirv_get_type(gen->module, type->element_type);
		WgvkSpvConstant *len = wgvk_spirv_get_constant(gen->module, type->length);
		emit(gen, "array<");
		emit_type(gen, elem);
		emit(gen, ", %u>", len ? len->value[0] : 1);
		break;
	}
	case WGVK_SPV_OP_TYPE_STRUCT:
		emit(gen, "Struct_%u", type->id);
		break;
	case WGVK_SPV_OP_TYPE_POINTER: {
		WgvkSpvType *elem = wgvk_spirv_get_type(gen->module, type->element_type);
		emit_type(gen, elem);
		break;
	}
	default:
		emit(gen, "f32");
		break;
	}
}

static const char *builtin_name(uint32_t builtin) {
	switch (builtin) {
	case WGVK_SPV_BUILTIN_POSITION:
		return "position";
	case WGVK_SPV_BUILTIN_VERTEX_INDEX:
		return "vertex_index";
	case WGVK_SPV_BUILTIN_INSTANCE_INDEX:
		return "instance_index";
	case WGVK_SPV_BUILTIN_FRONT_FACING:
		return "front_facing";
	case WGVK_SPV_BUILTIN_FRAG_COORD:
		return "position";
	case WGVK_SPV_BUILTIN_POINT_SIZE:
		return "point_size";
	case WGVK_SPV_BUILTIN_SAMPLE_ID:
		return "sample_index";
	case WGVK_SPV_BUILTIN_SAMPLE_MASK:
		return "sample_mask";
	case WGVK_SPV_BUILTIN_NUM_WORKGROUPS:
		return "num_workgroups";
	case WGVK_SPV_BUILTIN_WORKGROUP_ID:
		return "workgroup_id";
	case WGVK_SPV_BUILTIN_LOCAL_INVOCATION_ID:
		return "local_invocation_id";
	case WGVK_SPV_BUILTIN_GLOBAL_INVOCATION_ID:
		return "global_invocation_id";
	case WGVK_SPV_BUILTIN_LOCAL_INVOCATION_INDEX:
		return "local_invocation_index";
	default:
		return NULL;
	}
}

static const char *storage_class_to_address_space(uint32_t storage_class) {
	switch (storage_class) {
	case WGVK_SPV_STORAGE_CLASS_UNIFORM:
	case WGVK_SPV_STORAGE_CLASS_UNIFORM_CONSTANT:
		return "uniform";
	case WGVK_SPV_STORAGE_CLASS_STORAGE_BUFFER:
		return "storage";
	case WGVK_SPV_STORAGE_CLASS_INPUT:
	case WGVK_SPV_STORAGE_CLASS_OUTPUT:
		return NULL;
	case WGVK_SPV_STORAGE_CLASS_FUNCTION:
		return "function";
	case WGVK_SPV_STORAGE_CLASS_PRIVATE:
		return "private";
	case WGVK_SPV_STORAGE_CLASS_WORKGROUP:
		return "workgroup";
	case WGVK_SPV_STORAGE_CLASS_PUSH_CONSTANT:
		return "uniform";
	default:
		return NULL;
	}
}

static void emit_struct_definitions(WgvkWgslGenerator *gen) {
	WgvkSpvModule *mod = gen->module;

	for (uint32_t i = 0; i < mod->type_count; i++) {
		WgvkSpvType *t = &mod->types[i];
		if (t->op != WGVK_SPV_OP_TYPE_STRUCT)
			continue;

		emit(gen, "struct Struct_%u {\n", t->id);
		for (uint32_t m = 0; m < t->member_count && m < 16; m++) {
			WgvkSpvType *member_type = wgvk_spirv_get_type(mod, t->member_types[m]);
			emit(gen, "    member_%u: ", m);
			emit_type(gen, member_type);
			emit(gen, ",\n");
		}
		emit(gen, "};\n\n");
	}
}

static void emit_resource_bindings(WgvkWgslGenerator *gen) {
	WgvkSpvModule *mod = gen->module;

	for (uint32_t i = 0; i < mod->variable_count; i++) {
		WgvkSpvVariable *var = &mod->variables[i];

		uint32_t storage_class = var->storage_class;
		if (storage_class == WGVK_SPV_STORAGE_CLASS_INPUT ||
		    storage_class == WGVK_SPV_STORAGE_CLASS_OUTPUT) {
			continue;
		}

		WgvkSpvDecorationInfo *binding_dec =
		    wgvk_spirv_get_decoration(mod, var->id, WGVK_SPV_DECORATION_BINDING);
		WgvkSpvDecorationInfo *set_dec =
		    wgvk_spirv_get_decoration(mod, var->id, WGVK_SPV_DECORATION_DESCRIPTOR_SET);

		if (!binding_dec)
			continue;

		uint32_t binding = binding_dec->value;
		uint32_t set = set_dec ? set_dec->value : 0;

		WgvkSpvType *ptr_type = wgvk_spirv_get_type(mod, var->type_id);
		WgvkSpvType *var_type = ptr_type ? wgvk_spirv_get_type(mod, ptr_type->element_type) : NULL;

		emit(gen, "@group(%u) @binding(%u)\n", set, binding);

		const char *addr_space = storage_class_to_address_space(storage_class);
		if (!addr_space)
			addr_space = "uniform";

		emit(gen, "var");

		if (var_type &&
		    (var_type->op == WGVK_SPV_OP_TYPE_IMAGE || var_type->op == WGVK_SPV_OP_TYPE_SAMPLER ||
		     var_type->op == WGVK_SPV_OP_TYPE_SAMPLED_IMAGE)) {
		} else {
			emit(gen, "<%s>", addr_space);
		}

		emit(gen, " resource_%u_%u: ", set, binding);
		emit_type(gen, var_type);
		emit(gen, ";\n\n");
	}
}

static void emit_io_struct(WgvkWgslGenerator *gen, const char *name, uint32_t storage_class) {
	WgvkSpvModule *mod = gen->module;
	int has_members = 0;

	for (uint32_t i = 0; i < mod->variable_count; i++) {
		WgvkSpvVariable *var = &mod->variables[i];
		if (var->storage_class != storage_class)
			continue;

		WgvkSpvDecorationInfo *loc =
		    wgvk_spirv_get_decoration(mod, var->id, WGVK_SPV_DECORATION_LOCATION);
		WgvkSpvDecorationInfo *builtin =
		    wgvk_spirv_get_decoration(mod, var->id, WGVK_SPV_DECORATION_BUILTIN);
		if (!loc && !builtin)
			continue;
		has_members = 1;
		break;
	}

	if (!has_members)
		return;

	emit(gen, "struct %s {\n", name);

	for (uint32_t i = 0; i < mod->variable_count; i++) {
		WgvkSpvVariable *var = &mod->variables[i];
		if (var->storage_class != storage_class)
			continue;

		WgvkSpvDecorationInfo *loc =
		    wgvk_spirv_get_decoration(mod, var->id, WGVK_SPV_DECORATION_LOCATION);
		WgvkSpvDecorationInfo *builtin_dec =
		    wgvk_spirv_get_decoration(mod, var->id, WGVK_SPV_DECORATION_BUILTIN);

		WgvkSpvType *ptr_type = wgvk_spirv_get_type(mod, var->type_id);
		WgvkSpvType *var_type = ptr_type ? wgvk_spirv_get_type(mod, ptr_type->element_type) : NULL;

		if (builtin_dec) {
			const char *bname = builtin_name(builtin_dec->value);
			if (bname) {
				emit(gen, "    @builtin(%s) ", bname);
				emit(gen, "field_%u: ", var->id);
				emit_type(gen, var_type);
				emit(gen, ",\n");
			}
		} else if (loc) {
			emit(gen, "    @location(%u) field_%u: ", loc->value, var->id);
			emit_type(gen, var_type);
			emit(gen, ",\n");
		}
	}

	emit(gen, "};\n\n");
}

static void emit_local_variables(WgvkWgslGenerator *gen) {
	WgvkSpvModule *mod = gen->module;

	for (uint32_t i = 0; i < mod->variable_count; i++) {
		WgvkSpvVariable *var = &mod->variables[i];
		if (var->storage_class != WGVK_SPV_STORAGE_CLASS_FUNCTION)
			continue;

		WgvkSpvType *ptr_type = wgvk_spirv_get_type(mod, var->type_id);
		WgvkSpvType *var_type = ptr_type ? wgvk_spirv_get_type(mod, ptr_type->element_type) : NULL;

		emit(gen, "    var v%u: ", var->id);
		emit_type(gen, var_type);
		emit(gen, ";\n");
	}
}

static void emit_function_body(WgvkWgslGenerator *gen, WgvkSpvFunction *func);

static void emit_entry_function(WgvkWgslGenerator *gen) {
	WgvkSpvModule *mod = gen->module;
	uint32_t exec_model = (uint32_t)gen->exec_model;

	if (exec_model == WGVK_SPV_EXEC_MODEL_VERTEX) {
		emit(gen, "@vertex\n");
	} else if (exec_model == WGVK_SPV_EXEC_MODEL_FRAGMENT) {
		emit(gen, "@fragment\n");
	} else if (exec_model == WGVK_SPV_EXEC_MODEL_GL_COMPUTE) {
		emit(gen, "@compute @workgroup_size(1)\n");
	}

	const char *entry_name = wgvk_spirv_get_entry_name(mod, exec_model);

	int has_input_struct = 0;
	int has_output_struct = 0;

	for (uint32_t i = 0; i < mod->variable_count; i++) {
		WgvkSpvVariable *var = &mod->variables[i];
		WgvkSpvDecorationInfo *loc =
		    wgvk_spirv_get_decoration(mod, var->id, WGVK_SPV_DECORATION_LOCATION);
		WgvkSpvDecorationInfo *bi =
		    wgvk_spirv_get_decoration(mod, var->id, WGVK_SPV_DECORATION_BUILTIN);
		if (!loc && !bi)
			continue;
		if (var->storage_class == WGVK_SPV_STORAGE_CLASS_INPUT)
			has_input_struct = 1;
		if (var->storage_class == WGVK_SPV_STORAGE_CLASS_OUTPUT)
			has_output_struct = 1;
	}

	emit(gen, "fn %s(", entry_name);

	if (has_input_struct) {
		if (exec_model == WGVK_SPV_EXEC_MODEL_FRAGMENT) {
			emit(gen, "in: FragInput");
		} else {
			emit(gen, "in: VertexInput");
		}
	} else if (exec_model == WGVK_SPV_EXEC_MODEL_GL_COMPUTE) {
		emit(gen, "@builtin(global_invocation_id) global_id: vec3<u32>");
	}

	emit(gen, ")");

	if (exec_model != WGVK_SPV_EXEC_MODEL_GL_COMPUTE) {
		if (has_output_struct) {
			emit(gen, " -> VertexOutput");
		} else if (exec_model == WGVK_SPV_EXEC_MODEL_FRAGMENT) {
			emit(gen, " -> @location(0) vec4<f32>");
		} else {
			emit(gen, " -> @builtin(position) vec4<f32>");
		}
	}

	emit(gen, " {\n");

	emit_local_variables(gen);

	WgvkSpvFunction *entry_func = wgvk_spirv_get_function(mod, gen->entry_point_id);
	if (entry_func && entry_func->block_count > 0) {
		emit_function_body(gen, entry_func);
	} else {
		if (exec_model != WGVK_SPV_EXEC_MODEL_GL_COMPUTE) {
			if (has_output_struct) {
				emit(gen, "    var out: VertexOutput;\n");
				emit(gen, "    return out;\n");
			} else {
				emit(gen, "    return vec4<f32>(0.0, 0.0, 0.0, 1.0);\n");
			}
		}
	}

	emit(gen, "}\n");
}

static const char *opcode_to_wgsl(uint32_t opcode) {
	switch (opcode) {
	case WGVK_SPV_OP_FADD:
		return "+";
	case WGVK_SPV_OP_FSUB:
		return "-";
	case WGVK_SPV_OP_FMUL:
		return "*";
	case WGVK_SPV_OP_FDIV:
		return "/";
	case WGVK_SPV_OP_IADD:
		return "+";
	case WGVK_SPV_OP_ISUB:
		return "-";
	case WGVK_SPV_OP_IMUL:
		return "*";
	case WGVK_SPV_OP_SDIV:
		return "/";
	case WGVK_SPV_OP_UDIV:
		return "/";
	case WGVK_SPV_OP_SNEGATE:
		return "-";
	case WGVK_SPV_OP_FNEGATE:
		return "-";
	case WGVK_SPV_OP_BITWISE_OR:
		return "|";
	case WGVK_SPV_OP_BITWISE_XOR:
		return "^";
	case WGVK_SPV_OP_BITWISE_AND:
		return "&";
	case WGVK_SPV_OP_SHIFT_LEFT_LOGICAL:
		return "<<";
	case WGVK_SPV_OP_SHIFT_RIGHT_LOGICAL:
		return ">>";
	case WGVK_SPV_OP_LOGICAL_AND:
		return "&&";
	case WGVK_SPV_OP_LOGICAL_OR:
		return "||";
	case WGVK_SPV_OP_LOGICAL_EQUAL:
		return "==";
	case WGVK_SPV_OP_LOGICAL_NOT_EQUAL:
		return "!=";
	case WGVK_SPV_OP_F_ORD_EQUAL:
		return "==";
	case WGVK_SPV_OP_F_ORD_NOT_EQUAL:
		return "!=";
	case WGVK_SPV_OP_F_ORD_LESS_THAN:
		return "<";
	case WGVK_SPV_OP_F_ORD_GREATER_THAN:
		return ">";
	case WGVK_SPV_OP_F_ORD_LESS_THAN_EQUAL:
		return "<=";
	case WGVK_SPV_OP_F_ORD_GREATER_THAN_EQUAL:
		return ">=";
	default:
		return NULL;
	}
}

static void emit_instruction(WgvkWgslGenerator *gen, WgvkSpvInstruction *inst) {
	if (!inst)
		return;

	WgvkSpvModule *mod = gen->module;
	const char *op = opcode_to_wgsl(inst->opcode);

	if (op && inst->result_id && inst->operand_count >= 2) {
		WgvkSpvType *result_type = wgvk_spirv_get_type(mod, inst->result_type_id);
		emit(gen, "    var v%u: ", inst->result_id);
		emit_type(gen, result_type);
		if (inst->opcode == WGVK_SPV_OP_FNEGATE || inst->opcode == WGVK_SPV_OP_SNEGATE ||
		    inst->opcode == WGVK_SPV_OP_LOGICAL_NOT || inst->opcode == WGVK_SPV_OP_NOT) {
			emit(gen, " = %sv%u;\n", op, inst->operands[0]);
		} else {
			emit(gen, " = v%u %s v%u;\n", inst->operands[0], op, inst->operands[1]);
		}
		return;
	}

	switch (inst->opcode) {
	case WGVK_SPV_OP_LOAD: {
		if (inst->result_id && inst->operand_count >= 1) {
			WgvkSpvType *result_type = wgvk_spirv_get_type(mod, inst->result_type_id);
			emit(gen, "    let v%u: ", inst->result_id);
			emit_type(gen, result_type);
			emit(gen, " = v%u;\n", inst->operands[0]);
		}
		break;
	}
	case WGVK_SPV_OP_STORE: {
		if (inst->operand_count >= 2) {
			emit(gen, "    v%u = v%u;\n", inst->operands[0], inst->operands[1]);
		}
		break;
	}
	case WGVK_SPV_OP_ACCESS_CHAIN: {
		if (inst->result_id && inst->operand_count >= 2) {
			WgvkSpvType *result_type = wgvk_spirv_get_type(mod, inst->result_type_id);
			emit(gen, "    let v%u: ", inst->result_id);
			emit_type(gen, result_type);
			emit(gen, " = &v%u[", inst->operands[0]);
			for (uint32_t i = 1; i < inst->operand_count; i++) {
				if (i > 1)
					emit(gen, "][");
				emit(gen, "v%u", inst->operands[i]);
			}
			emit(gen, "];\n");
		}
		break;
	}
	case WGVK_SPV_OP_COMPOSITE_EXTRACT: {
		if (inst->result_id && inst->operand_count >= 2) {
			WgvkSpvType *result_type = wgvk_spirv_get_type(mod, inst->result_type_id);
			emit(gen, "    let v%u: ", inst->result_id);
			emit_type(gen, result_type);
			emit(gen, " = v%u", inst->operands[0]);
			for (uint32_t i = 1; i < inst->operand_count; i++) {
				emit(gen, "[%u]", inst->operands[i]);
			}
			emit(gen, ";\n");
		}
		break;
	}
	case WGVK_SPV_OP_DOT: {
		if (inst->result_id && inst->operand_count >= 2) {
			WgvkSpvType *result_type = wgvk_spirv_get_type(mod, inst->result_type_id);
			emit(gen, "    let v%u: ", inst->result_id);
			emit_type(gen, result_type);
			emit(gen, " = dot(v%u, v%u);\n", inst->operands[0], inst->operands[1]);
		}
		break;
	}
	case WGVK_SPV_OP_RETURN: {
		emit(gen, "    return;\n");
		break;
	}
	case WGVK_SPV_OP_RETURN_VALUE: {
		if (inst->operand_count >= 1) {
			emit(gen, "    return v%u;\n", inst->operands[0]);
		}
		break;
	}
	case WGVK_SPV_OP_BRANCH: {
		break;
	}
	default:
		WGVK_TRACE(WGVK_LOG_CAT_SHADER, "SPIR-V opcode %u not translated to WGSL (unsupported)",
		           inst->opcode);
		break;
	}
}

static void emit_function_body(WgvkWgslGenerator *gen, WgvkSpvFunction *func) {
	if (!func)
		return;

	for (uint32_t i = 0; i < func->block_count; i++) {
		WgvkSpvBlock *block = &func->blocks[i];
		for (uint32_t j = 0; j < block->instruction_count; j++) {
			emit_instruction(gen, &block->instructions[j]);
		}
	}
}

int wgvk_wgsl_init(WgvkWgslGenerator *gen, WgvkSpvModule *module, uint32_t exec_model) {
	if (!gen || !module)
		return -1;

	memset(gen, 0, sizeof(*gen));
	gen->module = module;
	gen->exec_model = (int)exec_model;

	for (uint32_t i = 0; i < module->entry_point_count; i++) {
		if (module->entry_points[i].exec_model == exec_model) {
			gen->entry_point_id = module->entry_points[i].entry_point_id;
			break;
		}
	}

	return 0;
}

char *wgvk_wgsl_generate(WgvkWgslGenerator *gen) {
	if (!gen || !gen->module)
		return NULL;

	emit(gen, "// Generated by webvulkan SPIR-V to WGSL translator\n\n");

	emit_struct_definitions(gen);
	if (gen->exec_model == WGVK_SPV_EXEC_MODEL_VERTEX) {
		emit_io_struct(gen, "VertexInput", WGVK_SPV_STORAGE_CLASS_INPUT);
		emit_io_struct(gen, "VertexOutput", WGVK_SPV_STORAGE_CLASS_OUTPUT);
	} else if (gen->exec_model == WGVK_SPV_EXEC_MODEL_FRAGMENT) {
		emit_io_struct(gen, "FragInput", WGVK_SPV_STORAGE_CLASS_INPUT);
		emit_io_struct(gen, "FragOutput", WGVK_SPV_STORAGE_CLASS_OUTPUT);
	}
	emit_resource_bindings(gen);
	emit_entry_function(gen);

	char *result = malloc(gen->cursor + 1);
	if (result) {
		memcpy(result, gen->buffer, gen->cursor);
		result[gen->cursor] = '\0';
	}
	return result;
}

void wgvk_wgsl_free(WgvkWgslGenerator *gen) {
	if (!gen)
		return;
	free(gen->buffer);
	gen->buffer = NULL;
	gen->cursor = 0;
	gen->capacity = 0;
}
