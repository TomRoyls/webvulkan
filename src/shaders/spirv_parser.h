#ifndef WGVK_SPIRV_H
#define WGVK_SPIRV_H

#include <stdint.h>
#include <stddef.h>

#define WGVK_SPV_MAGIC 0x07230203
#define WGVK_SPV_VERSION_1_0 0x00010000

typedef enum {
    WGVK_SPV_OP_NOP = 0,
    WGVK_SPV_OP_SOURCE = 3,
    WGVK_SPV_OP_EXTENSION = 10,
    WGVK_SPV_OP_EXT_INST_IMPORT = 11,
    WGVK_SPV_OP_MEMORY_MODEL = 14,
    WGVK_SPV_OP_ENTRY_POINT = 15,
    WGVK_SPV_OP_EXECUTION_MODE = 16,
    WGVK_SPV_OP_CAPABILITY = 17,
    WGVK_SPV_OP_TYPE_VOID = 19,
    WGVK_SPV_OP_TYPE_BOOL = 20,
    WGVK_SPV_OP_TYPE_INT = 21,
    WGVK_SPV_OP_TYPE_FLOAT = 22,
    WGVK_SPV_OP_TYPE_VECTOR = 23,
    WGVK_SPV_OP_TYPE_MATRIX = 24,
    WGVK_SPV_OP_TYPE_IMAGE = 25,
    WGVK_SPV_OP_TYPE_SAMPLER = 26,
    WGVK_SPV_OP_TYPE_SAMPLED_IMAGE = 27,
    WGVK_SPV_OP_TYPE_ARRAY = 28,
    WGVK_SPV_OP_TYPE_RUNTIME_ARRAY = 29,
    WGVK_SPV_OP_TYPE_STRUCT = 30,
    WGVK_SPV_OP_TYPE_POINTER = 32,
    WGVK_SPV_OP_TYPE_FUNCTION = 33,
    WGVK_SPV_OP_CONSTANT = 43,
    WGVK_SPV_OP_CONSTANT_COMPOSITE = 44,
    WGVK_SPV_OP_VARIABLE = 59,
    WGVK_SPV_OP_LOAD = 61,
    WGVK_SPV_OP_STORE = 62,
    WGVK_SPV_OP_ACCESS_CHAIN = 65,
    WGVK_SPV_OP_DECORATE = 71,
    WGVK_SPV_OP_MEMBER_DECORATE = 72,
    WGVK_SPV_OP_FUNCTION = 54,
    WGVK_SPV_OP_FUNCTION_PARAMETER = 55,
    WGVK_SPV_OP_FUNCTION_END = 56,
    WGVK_SPV_OP_CALL = 57,
    WGVK_SPV_OP_RETURN = 253,
    WGVK_SPV_OP_FADD = 126,
    WGVK_SPV_OP_FSUB = 129,
    WGVK_SPV_OP_FMUL = 132,
    WGVK_SPV_OP_FDIV = 135,
    WGVK_SPV_OP_RETURN_VALUE = 254,
    WGVK_SPV_OP_LABEL = 248,
    WGVK_SPV_OP_BRANCH = 249,
    WGVK_SPV_OP_BRANCH_CONDITIONAL = 250,
    WGVK_SPV_OP_LOOP_MERGE = 246,
    WGVK_SPV_OP_SELECTION_MERGE = 247,
    WGVK_SPV_OP_SWITCH = 251,
    WGVK_SPV_OP_VECTOR_TIMES_MATRIX = 136,
    WGVK_SPV_OP_MATRIX_TIMES_VECTOR = 137,
    WGVK_SPV_OP_MATRIX_TIMES_MATRIX = 138,
    WGVK_SPV_OP_COMPOSITE_CONSTRUCT = 80,
    WGVK_SPV_OP_COMPOSITE_EXTRACT = 81,
    WGVK_SPV_OP_COMPOSITE_INSERT = 82,
    WGVK_SPV_OP_COPY_OBJECT = 81,
    WGVK_SPV_OP_CONVERSION_F_TO_I = 124,
    WGVK_SPV_OP_CONVERSION_I_TO_F = 125,
} WgvkSpvOp;

typedef enum {
    WGVK_SPV_STORAGE_CLASS_UNIFORM_CONSTANT = 0,
    WGVK_SPV_STORAGE_CLASS_INPUT = 1,
    WGVK_SPV_STORAGE_CLASS_UNIFORM = 2,
    WGVK_SPV_STORAGE_CLASS_OUTPUT = 3,
    WGVK_SPV_STORAGE_CLASS_WORKGROUP = 4,
    WGVK_SPV_STORAGE_CLASS_CROSS_WORKGROUP = 5,
    WGVK_SPV_STORAGE_CLASS_PRIVATE = 6,
    WGVK_SPV_STORAGE_CLASS_FUNCTION = 7,
    WGVK_SPV_STORAGE_CLASS_GENERIC = 8,
    WGVK_SPV_STORAGE_CLASS_PUSH_CONSTANT = 9,
    WGVK_SPV_STORAGE_CLASS_ATOMIC_COUNTER = 10,
    WGVK_SPV_STORAGE_CLASS_IMAGE = 11,
    WGVK_SPV_STORAGE_CLASS_STORAGE_BUFFER = 12,
} WgvkSpvStorageClass;

typedef enum {
    WGVK_SPV_DECORATION_BLOCK = 2,
    WGVK_SPV_DECORATION_BUFFER_BLOCK = 3,
    WGVK_SPV_DECORATION_LOCATION = 30,
    WGVK_SPV_DECORATION_BINDING = 33,
    WGVK_SPV_DECORATION_DESCRIPTOR_SET = 34,
    WGVK_SPV_DECORATION_BUILTIN = 11,
    WGVK_SPV_DECORATION_OFFSET = 35,
    WGVK_SPV_DECORATION_ARRAY_STRIDE = 44,
    WGVK_SPV_DECORATION_MATRIX_STRIDE = 46,
    WGVK_SPV_DECORATION_NON_WRITABLE = 34,
} WgvkSpvDecoration;

typedef enum {
    WGVK_SPV_BUILTIN_POSITION = 0,
    WGVK_SPV_BUILTIN_POINT_SIZE = 1,
    WGVK_SPV_BUILTIN_CLIP_DISTANCE = 3,
    WGVK_SPV_BUILTIN_CULL_DISTANCE = 4,
    WGVK_SPV_BUILTIN_VERTEX_ID = 5,
    WGVK_SPV_BUILTIN_INSTANCE_ID = 6,
    WGVK_SPV_BUILTIN_PRIMITIVE_ID = 7,
    WGVK_SPV_BUILTIN_INVOCATION_ID = 8,
    WGVK_SPV_BUILTIN_LAYER = 9,
    WGVK_SPV_BUILTIN_VIEWPORT_INDEX = 10,
    WGVK_SPV_BUILTIN_TESS_LEVEL_OUTER = 11,
    WGVK_SPV_BUILTIN_TESS_LEVEL_INNER = 12,
    WGVK_SPV_BUILTIN_TESS_COORD = 13,
    WGVK_SPV_BUILTIN_PATCH_VERTICES = 14,
    WGVK_SPV_BUILTIN_FRAG_COORD = 15,
    WGVK_SPV_BUILTIN_FRONT_FACING = 16,
    WGVK_SPV_BUILTIN_POINT_COORD = 17,
    WGVK_SPV_BUILTIN_SAMPLE_ID = 18,
    WGVK_SPV_BUILTIN_SAMPLE_POS = 19,
    WGVK_SPV_BUILTIN_SAMPLE_MASK = 20,
    WGVK_SPV_BUILTIN_FRAG_DEPTH = 22,
    WGVK_SPV_BUILTIN_NUM_WORKGROUPS = 24,
    WGVK_SPV_BUILTIN_WORKGROUP_SIZE = 25,
    WGVK_SPV_BUILTIN_WORKGROUP_ID = 26,
    WGVK_SPV_BUILTIN_LOCAL_INVOCATION_ID = 27,
    WGVK_SPV_BUILTIN_GLOBAL_INVOCATION_ID = 28,
    WGVK_SPV_BUILTIN_LOCAL_INVOCATION_INDEX = 29,
    WGVK_SPV_BUILTIN_VERTEX_INDEX = 44,
    WGVK_SPV_BUILTIN_INSTANCE_INDEX = 45,
} WgvkSpvBuiltin;

typedef enum {
    WGVK_SPV_EXEC_MODEL_VERTEX = 0,
    WGVK_SPV_EXEC_MODEL_TESSELLATION_CONTROL = 1,
    WGVK_SPV_EXEC_MODEL_TESSELLATION_EVALUATION = 2,
    WGVK_SPV_EXEC_MODEL_GEOMETRY = 3,
    WGVK_SPV_EXEC_MODEL_FRAGMENT = 4,
    WGVK_SPV_EXEC_MODEL_GL_COMPUTE = 5,
    WGVK_SPV_EXEC_MODEL_KERNEL = 6,
    WGVK_SPV_EXEC_MODEL_TASK_NV = 5267,
    WGVK_SPV_EXEC_MODEL_MESH_NV = 5268,
    WGVK_SPV_EXEC_MODEL_RAY_GENERATION_NV = 5313,
    WGVK_SPV_EXEC_MODEL_INTERSECTION_NV = 5314,
    WGVK_SPV_EXEC_MODEL_ANY_HIT_NV = 5315,
    WGVK_SPV_EXEC_MODEL_CLOSEST_HIT_NV = 5316,
    WGVK_SPV_EXEC_MODEL_MISS_NV = 5317,
    WGVK_SPV_EXEC_MODEL_CALLABLE_NV = 5318,
} WgvkSpvExecutionModel;

#define WGVK_MAX_TYPES 1024
#define WGVK_MAX_CONSTANTS 512
#define WGVK_MAX_VARIABLES 256
#define WGVK_MAX_DECORATIONS 512
#define WGVK_MAX_ENTRY_POINTS 8
#define WGVK_MAX_NAME_LEN 256

typedef struct {
    uint32_t id;
    uint32_t op;
    uint32_t scalar_type;
    uint32_t width;
    uint32_t signedness;
    uint32_t vector_size;
    uint32_t matrix_cols;
    uint32_t matrix_rows;
    uint32_t element_type;
    uint32_t member_count;
    uint32_t member_types[16];
    uint32_t length;
    uint32_t storage_class;
    uint32_t return_type;
    uint32_t param_count;
    uint32_t param_types[8];
} WgvkSpvType;

typedef struct {
    uint32_t id;
    uint32_t type_id;
    uint32_t value[4];
    float fvalue[4];
    uint32_t component_count;
    uint32_t component_ids[4];
} WgvkSpvConstant;

typedef struct {
    uint32_t id;
    uint32_t type_id;
    uint32_t storage_class;
    const char* name;
} WgvkSpvVariable;

typedef struct {
    uint32_t target_id;
    uint32_t decoration;
    uint32_t value;
    uint32_t member;
} WgvkSpvDecorationInfo;

typedef struct {
    uint32_t exec_model;
    uint32_t entry_point_id;
    const char* name;
    uint32_t interface_count;
    uint32_t interface_ids[32];
} WgvkSpvEntryPoint;

typedef struct {
    const uint32_t* words;
    size_t word_count;
    size_t cursor;
    
    WgvkSpvType types[WGVK_MAX_TYPES];
    uint32_t type_count;
    
    WgvkSpvConstant constants[WGVK_MAX_CONSTANTS];
    uint32_t constant_count;
    
    WgvkSpvVariable variables[WGVK_MAX_VARIABLES];
    uint32_t variable_count;
    
    WgvkSpvDecorationInfo decorations[WGVK_MAX_DECORATIONS];
    uint32_t decoration_count;
    
    WgvkSpvEntryPoint entry_points[WGVK_MAX_ENTRY_POINTS];
    uint32_t entry_point_count;
    
    char string_buffer[8192];
    size_t string_cursor;
} WgvkSpvModule;

int wgvk_spirv_parse(WgvkSpvModule* module, const uint32_t* code, size_t size);
void wgvk_spirv_free(WgvkSpvModule* module);
WgvkSpvType* wgvk_spirv_get_type(WgvkSpvModule* module, uint32_t id);
WgvkSpvConstant* wgvk_spirv_get_constant(WgvkSpvModule* module, uint32_t id);
WgvkSpvVariable* wgvk_spirv_get_variable(WgvkSpvModule* module, uint32_t id);
WgvkSpvDecorationInfo* wgvk_spirv_get_decoration(WgvkSpvModule* module, uint32_t id, uint32_t decoration);
const char* wgvk_spirv_get_entry_name(WgvkSpvModule* module, uint32_t exec_model);

#endif
