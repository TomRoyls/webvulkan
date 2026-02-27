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

static void emit_type(WgvkWgslGenerator* gen, WgvkSpvType* type);

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
                emit(gen, type->signedness ? "i32" : "u32");
            } else {
                emit(gen, "i32");
            }
            break;
        case WGVK_SPV_OP_TYPE_FLOAT:
            if (type->width == 32) {
                emit(gen, "f32");
            } else if (type->width == 64) {
                emit(gen, "f32");
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
            emit(gen, "Struct_%u", type->id);
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
        case WGVK_SPV_BUILTIN_POSITION:             return "position";
        case WGVK_SPV_BUILTIN_VERTEX_INDEX:          return "vertex_index";
        case WGVK_SPV_BUILTIN_INSTANCE_INDEX:        return "instance_index";
        case WGVK_SPV_BUILTIN_FRONT_FACING:          return "front_facing";
        case WGVK_SPV_BUILTIN_FRAG_COORD:            return "position";
        case WGVK_SPV_BUILTIN_POINT_SIZE:            return "point_size";
        case WGVK_SPV_BUILTIN_SAMPLE_ID:             return "sample_index";
        case WGVK_SPV_BUILTIN_SAMPLE_MASK:           return "sample_mask";
        case WGVK_SPV_BUILTIN_NUM_WORKGROUPS:        return "num_workgroups";
        case WGVK_SPV_BUILTIN_WORKGROUP_ID:          return "workgroup_id";
        case WGVK_SPV_BUILTIN_LOCAL_INVOCATION_ID:   return "local_invocation_id";
        case WGVK_SPV_BUILTIN_GLOBAL_INVOCATION_ID:  return "global_invocation_id";
        case WGVK_SPV_BUILTIN_LOCAL_INVOCATION_INDEX: return "local_invocation_index";
        default:                                      return NULL;
    }
}

static const char* storage_class_to_address_space(uint32_t storage_class) {
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

static void emit_struct_definitions(WgvkWgslGenerator* gen) {
    WgvkSpvModule* mod = gen->module;

    for (uint32_t i = 0; i < mod->type_count; i++) {
        WgvkSpvType* t = &mod->types[i];
        if (t->op != WGVK_SPV_OP_TYPE_STRUCT) continue;

        emit(gen, "struct Struct_%u {\n", t->id);
        for (uint32_t m = 0; m < t->member_count && m < 16; m++) {
            WgvkSpvType* member_type = wgvk_spirv_get_type(mod, t->member_types[m]);
            emit(gen, "    member_%u: ", m);
            emit_type(gen, member_type);
            emit(gen, ",\n");
        }
        emit(gen, "};\n\n");
    }
}

static void emit_resource_bindings(WgvkWgslGenerator* gen) {
    WgvkSpvModule* mod = gen->module;

    for (uint32_t i = 0; i < mod->variable_count; i++) {
        WgvkSpvVariable* var = &mod->variables[i];

        uint32_t storage_class = var->storage_class;
        if (storage_class == WGVK_SPV_STORAGE_CLASS_INPUT ||
            storage_class == WGVK_SPV_STORAGE_CLASS_OUTPUT) {
            continue;
        }

        WgvkSpvDecorationInfo* binding_dec =
            wgvk_spirv_get_decoration(mod, var->id, WGVK_SPV_DECORATION_BINDING);
        WgvkSpvDecorationInfo* set_dec =
            wgvk_spirv_get_decoration(mod, var->id, WGVK_SPV_DECORATION_DESCRIPTOR_SET);

        if (!binding_dec) continue;

        uint32_t binding = binding_dec->value;
        uint32_t set     = set_dec ? set_dec->value : 0;

        WgvkSpvType* ptr_type = wgvk_spirv_get_type(mod, var->type_id);
        WgvkSpvType* var_type = ptr_type
            ? wgvk_spirv_get_type(mod, ptr_type->element_type)
            : NULL;

        emit(gen, "@group(%u) @binding(%u)\n", set, binding);

        const char* addr_space = storage_class_to_address_space(storage_class);
        if (!addr_space) addr_space = "uniform";

        emit(gen, "var");

        if (var_type && (var_type->op == WGVK_SPV_OP_TYPE_IMAGE ||
                         var_type->op == WGVK_SPV_OP_TYPE_SAMPLER ||
                         var_type->op == WGVK_SPV_OP_TYPE_SAMPLED_IMAGE)) {
        } else {
            emit(gen, "<%s>", addr_space);
        }

        emit(gen, " resource_%u_%u: ", set, binding);
        emit_type(gen, var_type);
        emit(gen, ";\n\n");
    }
}

static void emit_io_struct(WgvkWgslGenerator* gen, const char* name,
                            uint32_t storage_class) {
    WgvkSpvModule* mod = gen->module;
    int has_members = 0;

    for (uint32_t i = 0; i < mod->variable_count; i++) {
        WgvkSpvVariable* var = &mod->variables[i];
        if (var->storage_class != storage_class) continue;

        WgvkSpvDecorationInfo* loc =
            wgvk_spirv_get_decoration(mod, var->id, WGVK_SPV_DECORATION_LOCATION);
        WgvkSpvDecorationInfo* builtin =
            wgvk_spirv_get_decoration(mod, var->id, WGVK_SPV_DECORATION_BUILTIN);
        if (!loc && !builtin) continue;
        has_members = 1;
        break;
    }

    if (!has_members) return;

    emit(gen, "struct %s {\n", name);

    for (uint32_t i = 0; i < mod->variable_count; i++) {
        WgvkSpvVariable* var = &mod->variables[i];
        if (var->storage_class != storage_class) continue;

        WgvkSpvDecorationInfo* loc =
            wgvk_spirv_get_decoration(mod, var->id, WGVK_SPV_DECORATION_LOCATION);
        WgvkSpvDecorationInfo* builtin_dec =
            wgvk_spirv_get_decoration(mod, var->id, WGVK_SPV_DECORATION_BUILTIN);

        WgvkSpvType* ptr_type = wgvk_spirv_get_type(mod, var->type_id);
        WgvkSpvType* var_type = ptr_type
            ? wgvk_spirv_get_type(mod, ptr_type->element_type)
            : NULL;

        if (builtin_dec) {
            const char* bname = builtin_name(builtin_dec->value);
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

static void emit_entry_function(WgvkWgslGenerator* gen) {
    WgvkSpvModule* mod = gen->module;
    uint32_t exec_model = (uint32_t)gen->exec_model;

    if (exec_model == WGVK_SPV_EXEC_MODEL_VERTEX) {
        emit(gen, "@vertex\n");
    } else if (exec_model == WGVK_SPV_EXEC_MODEL_FRAGMENT) {
        emit(gen, "@fragment\n");
    } else if (exec_model == WGVK_SPV_EXEC_MODEL_GL_COMPUTE) {
        emit(gen, "@compute @workgroup_size(1)\n");
    }

    const char* entry_name = wgvk_spirv_get_entry_name(mod, exec_model);

    int has_input_struct  = 0;
    int has_output_struct = 0;

    for (uint32_t i = 0; i < mod->variable_count; i++) {
        WgvkSpvVariable* var = &mod->variables[i];
        WgvkSpvDecorationInfo* loc =
            wgvk_spirv_get_decoration(mod, var->id, WGVK_SPV_DECORATION_LOCATION);
        WgvkSpvDecorationInfo* bi =
            wgvk_spirv_get_decoration(mod, var->id, WGVK_SPV_DECORATION_BUILTIN);
        if (!loc && !bi) continue;
        if (var->storage_class == WGVK_SPV_STORAGE_CLASS_INPUT)  has_input_struct  = 1;
        if (var->storage_class == WGVK_SPV_STORAGE_CLASS_OUTPUT) has_output_struct = 1;
    }

    emit(gen, "fn %s(", entry_name);

    if (has_input_struct) {
        emit(gen, "in: VertexInput");
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

    if (exec_model != WGVK_SPV_EXEC_MODEL_GL_COMPUTE) {
        if (has_output_struct) {
            emit(gen, "    var out: VertexOutput;\n");
            emit(gen, "    return out;\n");
        } else {
            emit(gen, "    return vec4<f32>(0.0, 0.0, 0.0, 1.0);\n");
        }
    }

    emit(gen, "}\n");
}

int wgvk_wgsl_init(WgvkWgslGenerator* gen, WgvkSpvModule* module, uint32_t exec_model) {
    if (!gen || !module) return -1;

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

char* wgvk_wgsl_generate(WgvkWgslGenerator* gen) {
    if (!gen || !gen->module) return NULL;

    emit(gen, "// Generated by webvulkan SPIR-V to WGSL translator\n\n");

    emit_struct_definitions(gen);
    if (gen->exec_model == WGVK_SPV_EXEC_MODEL_VERTEX) {
        emit_io_struct(gen, "VertexInput",  WGVK_SPV_STORAGE_CLASS_INPUT);
        emit_io_struct(gen, "VertexOutput", WGVK_SPV_STORAGE_CLASS_OUTPUT);
    } else if (gen->exec_model == WGVK_SPV_EXEC_MODEL_FRAGMENT) {
        emit_io_struct(gen, "FragInput",  WGVK_SPV_STORAGE_CLASS_INPUT);
        emit_io_struct(gen, "FragOutput", WGVK_SPV_STORAGE_CLASS_OUTPUT);
    }
    emit_resource_bindings(gen);
    emit_entry_function(gen);

    char* result = malloc(gen->cursor + 1);
    if (result) {
        memcpy(result, gen->buffer, gen->cursor);
        result[gen->cursor] = '\0';
    }
    return result;
}

void wgvk_wgsl_free(WgvkWgslGenerator* gen) {
    (void)gen;
}
