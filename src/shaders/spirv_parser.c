#include "spirv_parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static uint32_t read_word(WgvkSpvModule* module) {
    if (module->cursor >= module->word_count) return 0;
    return module->words[module->cursor++];
}

static const char* read_string(WgvkSpvModule* module, size_t* out_len) {
    size_t start = module->string_cursor;
    char* dest = module->string_buffer + start;
    size_t max_len = sizeof(module->string_buffer) - start;
    
    size_t len = 0;
    while (len < max_len) {
        uint32_t word = module->words[module->cursor];
        module->cursor++;
        
        for (int i = 0; i < 4 && len < max_len; i++) {
            char c = (word >> (i * 8)) & 0xFF;
            if (c == '\0') {
                dest[len] = '\0';
                if (out_len) *out_len = len;
                module->string_cursor += len + 1;
                return dest;
            }
            dest[len++] = c;
        }
    }
    
    dest[len] = '\0';
    if (out_len) *out_len = len;
    module->string_cursor += len + 1;
    return dest;
}

static void skip_instruction(WgvkSpvModule* module, uint16_t word_count) {
    module->cursor += word_count - 1;
}

int wgvk_spirv_parse(WgvkSpvModule* module, const uint32_t* code, size_t size) {
    if (!module || !code || size < 5) return -1;
    
    memset(module, 0, sizeof(*module));
    module->words = code;
    module->word_count = size / 4;
    module->cursor = 0;
    
    uint32_t magic = read_word(module);
    if (magic != WGVK_SPV_MAGIC) return -1;
    
    uint32_t version = read_word(module);
    (void)version;
    
    read_word(module);
    read_word(module);
    read_word(module);
    
    while (module->cursor < module->word_count) {
        size_t inst_start = module->cursor;
        uint32_t word = read_word(module);
        uint16_t opcode = word & 0xFFFF;
        uint16_t word_count = (word >> 16) & 0xFFFF;
        
        if (word_count == 0) word_count = 1;
        
        switch (opcode) {
            case WGVK_SPV_OP_CAPABILITY:
            case WGVK_SPV_OP_EXTENSION:
            case WGVK_SPV_OP_EXT_INST_IMPORT:
            case WGVK_SPV_OP_MEMORY_MODEL:
            case WGVK_SPV_OP_SOURCE:
            case WGVK_SPV_OP_EXECUTION_MODE:
                skip_instruction(module, word_count);
                break;
                
            case WGVK_SPV_OP_ENTRY_POINT: {
                if (module->entry_point_count < WGVK_MAX_ENTRY_POINTS) {
                    WgvkSpvEntryPoint* ep = &module->entry_points[module->entry_point_count++];
                    ep->exec_model = read_word(module);
                    ep->entry_point_id = read_word(module);
                    ep->name = read_string(module, NULL);
                    
                    size_t remaining = word_count - (module->cursor - inst_start);
                    ep->interface_count = remaining;
                    for (size_t i = 0; i < remaining && i < 32; i++) {
                        ep->interface_ids[i] = read_word(module);
                    }
                } else {
                    skip_instruction(module, word_count);
                }
                break;
            }
            
            case WGVK_SPV_OP_DECORATE: {
                if (module->decoration_count < WGVK_MAX_DECORATIONS && word_count >= 3) {
                    WgvkSpvDecorationInfo* d = &module->decorations[module->decoration_count++];
                    d->target_id = read_word(module);
                    d->decoration = read_word(module);
                    d->member = 0xFFFFFFFF;
                    d->value = (word_count >= 4) ? read_word(module) : 0;
                } else {
                    skip_instruction(module, word_count);
                }
                break;
            }
            
            case WGVK_SPV_OP_MEMBER_DECORATE: {
                if (module->decoration_count < WGVK_MAX_DECORATIONS && word_count >= 4) {
                    WgvkSpvDecorationInfo* d = &module->decorations[module->decoration_count++];
                    d->target_id = read_word(module);
                    d->member = read_word(module);
                    d->decoration = read_word(module);
                    d->value = (word_count >= 5) ? read_word(module) : 0;
                } else {
                    skip_instruction(module, word_count);
                }
                break;
            }
            
            case WGVK_SPV_OP_TYPE_VOID: {
                if (module->type_count < WGVK_MAX_TYPES && word_count >= 2) {
                    WgvkSpvType* t = &module->types[module->type_count++];
                    t->id = read_word(module);
                    t->op = opcode;
                } else {
                    skip_instruction(module, word_count);
                }
                break;
            }
            
            case WGVK_SPV_OP_TYPE_BOOL: {
                if (module->type_count < WGVK_MAX_TYPES && word_count >= 2) {
                    WgvkSpvType* t = &module->types[module->type_count++];
                    t->id = read_word(module);
                    t->op = opcode;
                } else {
                    skip_instruction(module, word_count);
                }
                break;
            }
            
            case WGVK_SPV_OP_TYPE_INT: {
                if (module->type_count < WGVK_MAX_TYPES && word_count >= 4) {
                    WgvkSpvType* t = &module->types[module->type_count++];
                    t->id = read_word(module);
                    t->op = opcode;
                    t->width = read_word(module);
                    t->signedness = read_word(module);
                } else {
                    skip_instruction(module, word_count);
                }
                break;
            }
            
            case WGVK_SPV_OP_TYPE_FLOAT: {
                if (module->type_count < WGVK_MAX_TYPES && word_count >= 3) {
                    WgvkSpvType* t = &module->types[module->type_count++];
                    t->id = read_word(module);
                    t->op = opcode;
                    t->width = read_word(module);
                } else {
                    skip_instruction(module, word_count);
                }
                break;
            }
            
            case WGVK_SPV_OP_TYPE_VECTOR: {
                if (module->type_count < WGVK_MAX_TYPES && word_count >= 4) {
                    WgvkSpvType* t = &module->types[module->type_count++];
                    t->id = read_word(module);
                    t->op = opcode;
                    t->element_type = read_word(module);
                    t->vector_size = read_word(module);
                } else {
                    skip_instruction(module, word_count);
                }
                break;
            }
            
            case WGVK_SPV_OP_TYPE_MATRIX: {
                if (module->type_count < WGVK_MAX_TYPES && word_count >= 4) {
                    WgvkSpvType* t = &module->types[module->type_count++];
                    t->id = read_word(module);
                    t->op = opcode;
                    t->element_type = read_word(module);
                    t->matrix_cols = read_word(module);
                } else {
                    skip_instruction(module, word_count);
                }
                break;
            }
            
            case WGVK_SPV_OP_TYPE_ARRAY: {
                if (module->type_count < WGVK_MAX_TYPES && word_count >= 4) {
                    WgvkSpvType* t = &module->types[module->type_count++];
                    t->id = read_word(module);
                    t->op = opcode;
                    t->element_type = read_word(module);
                    t->length = read_word(module);
                } else {
                    skip_instruction(module, word_count);
                }
                break;
            }
            
            case WGVK_SPV_OP_TYPE_STRUCT: {
                if (module->type_count < WGVK_MAX_TYPES && word_count >= 2) {
                    WgvkSpvType* t = &module->types[module->type_count++];
                    t->id = read_word(module);
                    t->op = opcode;
                    t->member_count = word_count - 2;
                    for (uint32_t i = 0; i < t->member_count && i < 16; i++) {
                        t->member_types[i] = read_word(module);
                    }
                } else {
                    skip_instruction(module, word_count);
                }
                break;
            }
            
            case WGVK_SPV_OP_TYPE_POINTER: {
                if (module->type_count < WGVK_MAX_TYPES && word_count >= 4) {
                    WgvkSpvType* t = &module->types[module->type_count++];
                    t->id = read_word(module);
                    t->op = opcode;
                    t->storage_class = read_word(module);
                    t->element_type = read_word(module);
                } else {
                    skip_instruction(module, word_count);
                }
                break;
            }
            
            case WGVK_SPV_OP_TYPE_FUNCTION: {
                if (module->type_count < WGVK_MAX_TYPES && word_count >= 3) {
                    WgvkSpvType* t = &module->types[module->type_count++];
                    t->id = read_word(module);
                    t->op = opcode;
                    t->return_type = read_word(module);
                    t->param_count = word_count - 3;
                    for (uint32_t i = 0; i < t->param_count && i < 8; i++) {
                        t->param_types[i] = read_word(module);
                    }
                } else {
                    skip_instruction(module, word_count);
                }
                break;
            }
            
            case WGVK_SPV_OP_CONSTANT: {
                if (module->constant_count < WGVK_MAX_CONSTANTS && word_count >= 4) {
                    WgvkSpvConstant* c = &module->constants[module->constant_count++];
                    c->type_id = read_word(module);
                    c->id = read_word(module);
                    
                    size_t value_words = word_count - 3;
                    for (size_t i = 0; i < value_words && i < 4; i++) {
                        c->value[i] = read_word(module);
                    }
                    
                    if (value_words == 1) {
                        c->fvalue[0] = *(float*)&c->value[0];
                    } else if (value_words == 2) {
                        uint64_t bits = ((uint64_t)c->value[1] << 32) | c->value[0];
                        double d = *(double*)&bits;
                        c->fvalue[0] = (float)d;
                    }
                } else {
                    skip_instruction(module, word_count);
                }
                break;
            }
            
            case WGVK_SPV_OP_CONSTANT_COMPOSITE: {
                if (module->constant_count < WGVK_MAX_CONSTANTS && word_count >= 4) {
                    WgvkSpvConstant* c = &module->constants[module->constant_count++];
                    c->type_id = read_word(module);
                    c->id = read_word(module);
                    c->component_count = word_count - 3;
                    for (uint32_t i = 0; i < c->component_count && i < 4; i++) {
                        c->component_ids[i] = read_word(module);
                    }
                } else {
                    skip_instruction(module, word_count);
                }
                break;
            }
            
            case WGVK_SPV_OP_VARIABLE: {
                if (module->variable_count < WGVK_MAX_VARIABLES && word_count >= 4) {
                    WgvkSpvVariable* v = &module->variables[module->variable_count++];
                    v->type_id = read_word(module);
                    v->storage_class = read_word(module);
                    v->id = read_word(module);
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

void wgvk_spirv_free(WgvkSpvModule* module) {
    (void)module;
}

WgvkSpvType* wgvk_spirv_get_type(WgvkSpvModule* module, uint32_t id) {
    for (uint32_t i = 0; i < module->type_count; i++) {
        if (module->types[i].id == id) {
            return &module->types[i];
        }
    }
    return NULL;
}

WgvkSpvConstant* wgvk_spirv_get_constant(WgvkSpvModule* module, uint32_t id) {
    for (uint32_t i = 0; i < module->constant_count; i++) {
        if (module->constants[i].id == id) {
            return &module->constants[i];
        }
    }
    return NULL;
}

WgvkSpvVariable* wgvk_spirv_get_variable(WgvkSpvModule* module, uint32_t id) {
    for (uint32_t i = 0; i < module->variable_count; i++) {
        if (module->variables[i].id == id) {
            return &module->variables[i];
        }
    }
    return NULL;
}

WgvkSpvDecorationInfo* wgvk_spirv_get_decoration(WgvkSpvModule* module, uint32_t id, uint32_t decoration) {
    for (uint32_t i = 0; i < module->decoration_count; i++) {
        if (module->decorations[i].target_id == id && module->decorations[i].decoration == decoration) {
            return &module->decorations[i];
        }
    }
    return NULL;
}

const char* wgvk_spirv_get_entry_name(WgvkSpvModule* module, uint32_t exec_model) {
    for (uint32_t i = 0; i < module->entry_point_count; i++) {
        if (module->entry_points[i].exec_model == exec_model) {
            return module->entry_points[i].name;
        }
    }
    return "main";
}
