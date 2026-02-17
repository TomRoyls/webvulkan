#include "wgsl_gen.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

static void emit(WgvkWgslGenerator* gen, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    size_t remaining = WGVK_WGSL_BUFFER_SIZE - gen->cursor;
    int written = vsnprintf(gen->buffer + gen->cursor, remaining, fmt, args);
    va_end(args);
    if (written > 0 && (size_t)written < remaining) {
        gen->cursor += written;
    }
}

static void emit_type(WgvkWgslGenerator* gen, WgvkSpvType* type) {
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
                emit(gen, type->signedness ? "i64" : "u64");
            } else {
                emit(gen, "i32");
            }
            break;
        case WGVK_SPV_OP_TYPE_FLOAT:
            if (type->width == 32) {
                emit(gen, "f32");
            } else if (type->width == 64) {
                emit(gen, "f64");
            } else {
                emit(gen, "f32");
            }
            break;
        case WGVK_SPV_OP_TYPE_VECTOR: {
            WgvkSpvType* elem = wgvk_spirv_get_type(gen->module, type->element_type);
            if (elem && elem->op == WGVK_SPV_OP_TYPE_FLOAT) {
                emit(gen, "vec%u<f32>", type->vector_size);
            } else if (elem && elem->op == WGVK_SPV_OP_TYPE_INT) {
                emit(gen, "vec%u<%s>", type->vector_size, 
                     elem->signedness ? "i32" : "u32");
            } else {
                emit(gen, "vec%u<f32>", type->vector_size);
            }
            break;
        }
        case WGVK_SPV_OP_TYPE_MATRIX: {
            WgvkSpvType* elem = wgvk_spirv_get_type(gen->module, type->element_type);
            if (elem && elem->op == WGVK_SPV_OP_TYPE_VECTOR) {
                emit(gen, "mat%ux%u<f32>", type->matrix_cols, elem->vector_size);
            } else {
                emit(gen, "mat4x4<f32>");
            }
            break;
        }
        case WGVK_SPV_OP_TYPE_ARRAY: {
            WgvkSpvType* elem = wgvk_spirv_get_type(gen->module, type->element_type);
            WgvkSpvConstant* len = wgvk_spirv_get_constant(gen->module, type->length);
            emit(gen, "array<");
            emit_type(gen, elem);
            emit(gen, ", %u>", len ? len->value[0] : 1);
            break;
        }
        case WGVK_SPV_OP_TYPE_STRUCT:
            emit(gen, "struct_%u", type->id);
            break;
        case WGVK_SPV_OP_TYPE_POINTER: {
            WgvkSpvType* elem = wgvk_spirv_get_type(gen->module, type->element_type);
            emit_type(gen, elem);
            break;
        }
        default:
            emit(gen, "f32");
            break;
    }
}

static const char* builtin_name(uint32_t builtin) {
    switch (builtin) {
        case WGVK_SPV_BUILTIN_POSITION: return "position";
        case WGVK_SPV_BUILTIN_VERTEX_INDEX: return "vertex_index";
        case WGVK_SPV_BUILTIN_INSTANCE_INDEX: return "instance_index";
        case WGVK_SPV_BUILTIN_FRONT_FACING: return "front_facing";
        case WGVK_SPV_BUILTIN_FRAG_COORD: return "position";
        case WGVK_SPV_BUILTIN_POINT_SIZE: return "point_size";
        case WGVK_SPV_BUILTIN_SAMPLE_ID: return "sample_index";
        case WGVK_SPV_BUILTIN_SAMPLE_MASK: return "sample_mask";
        case WGVK_SPV_BUILTIN_NUM_WORKGROUPS: return "num_workgroups";
        case WGVK_SPV_BUILTIN_WORKGROUP_ID: return "workgroup_id";
        case WGVK_SPV_BUILTIN_LOCAL_INVOCATION_ID: return "local_invocation_id";
        case WGVK_SPV_BUILTIN_GLOBAL_INVOCATION_ID: return "global_invocation_id";
        case WGVK_SPV_BUILTIN_LOCAL_INVOCATION_INDEX: return "local_invocation_index";
        default: return "position";
    }
}

static const char* storage_class_to_address_space(uint32_t storage_class) {
    switch (storage_class) {
        case WGVK_SPV_STORAGE_CLASS_UNIFORM:
        case WGVK_SPV_STORAGE_CLASS_UNIFORM_CONSTANT:
        case WGVK_SPV_STORAGE_CLASS_STORAGE_BUFFER:
            return "uniform";
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
            return "push_constant";
        default:
            return NULL;
    }
}

int wgvk_wgsl_init(WgvkWgslGenerator* gen, WgvkSpvModule* module, uint32_t exec_model) {
    if (!gen || !module) return -1;
    
    memset(gen, 0, sizeof(*gen));
    gen->module = module;
    gen->exec_model = exec_model;
    
    for (uint32_t i = 0; i < module->entry_point_count; i++) {
        if (module->entry_points[i].exec_model == exec_model) {
            gen->entry_point_id = module->entry_points[i].entry_point_id;
            break;
        }
    }
    
    return 0;
}

char* wgvk_wgsl_generate(WgvkWgslGenerator* gen) {
    if (!gen || !gen->module) return NULL;
    
    emit(gen, "// Generated by webvulkan SPIR-V to WGSL translator\n\n");
    
    if (gen->exec_model == WGVK_SPV_EXEC_MODEL_VERTEX) {
        emit(gen, "@vertex\n");
    } else if (gen->exec_model == WGVK_SPV_EXEC_MODEL_FRAGMENT) {
        emit(gen, "@fragment\n");
    } else if (gen->exec_model == WGVK_SPV_EXEC_MODEL_GL_COMPUTE) {
        emit(gen, "@compute @workgroup_size(1)\n");
    }
    
    emit(gen, "fn main() -> @location(0) vec4<f32> {\n");
    emit(gen, "    return vec4<f32>(0.0, 0.0, 0.0, 1.0);\n");
    emit(gen, "}\n");
    
    char* result = malloc(gen->cursor + 1);
    if (result) {
        memcpy(result, gen->buffer, gen->cursor + 1);
    }
    return result;
}

void wgvk_wgsl_free(WgvkWgslGenerator* gen) {
    (void)gen;
}
