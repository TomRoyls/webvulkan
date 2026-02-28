#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/shaders/spirv_parser.h"
#include "../src/shaders/wgsl_gen.h"

static void test_parser_init(void) {
    WgvkSpvModule* module = calloc(1, sizeof(WgvkSpvModule));
    if (!module) { printf("[SKIP] test_parser_init (OOM)\n"); return; }
    
    int result = wgvk_spirv_parse(module, NULL, 0);
    assert(result == -1);
    printf("[PASS] test_parser_init\n");
    free(module);
}

static void test_magic_number(void) {
    assert(WGVK_SPV_MAGIC == 0x07230203);
    printf("[PASS] test_magic_number\n");
}

static void test_type_structures(void) {
    WgvkSpvModule* module = calloc(1, sizeof(WgvkSpvModule));
    if (!module) { printf("[SKIP] test_type_structures (OOM)\n"); return; }
    
    assert(module->type_count == 0);
    assert(module->constant_count == 0);
    assert(module->variable_count == 0);
    
    printf("[PASS] test_type_structures\n");
    free(module);
}

/*
 * Minimal SPIR-V for a vertex shader:
 *   OpCapability Shader               ; word 0x00020011
 *   OpMemoryModel Logical GLSL450      ; word 0x0003000E
 *   OpEntryPoint Vertex %1 "main"      ; sets exec_model=0, entry_point_id=1, name="main"
 *   OpTypeVoid %2                      ; type id=2
 *   OpTypeFunction %3 %2               ; type id=3, return=2
 *   OpFunction %2 %1 None %3           ; function id=1
 *   OpLabel %4                         ; block label id=4
 *   OpReturn                           ; terminates block
 *   OpFunctionEnd                      ; ends function
 */
static void test_simple_vertex_shader(void) {
    static const uint32_t vertex_spv[] = {
        /* Magic, Version, Generator, Bound, Schema */
        0x07230203, 0x00010000, 0x00000000, 0x00000005, 0x00000000,
        /* OpCapability Shader (word_count=2, opcode=17) */
        0x00020011,  0x00000001,
        /* OpMemoryModel Logical GLSL450 (word_count=3, opcode=14) */
        0x0003000E,  0x00000000,  0x00000001,
        /* OpEntryPoint Vertex %1 "main" (word_count=5, opcode=15): exec_model, id, "main"\0 pad */
        0x0005000F,  0x00000000,  0x00000001,  0x6E69616D,  0x00000000,
        /* OpTypeVoid %2 (word_count=2, opcode=19) */
        0x00020013,  0x00000002,
        /* OpTypeFunction %3 %2 (word_count=3, opcode=33) */
        0x00030021,  0x00000003,  0x00000002,
        /* OpFunction %2 %1 None %3 (word_count=5, opcode=54) */
        0x00050036,  0x00000002,  0x00000001,  0x00000000,  0x00000003,
        /* OpLabel %4 (word_count=2, opcode=248) */
        0x000200F8,  0x00000004,
        /* OpReturn (word_count=1, opcode=253) */
        0x000100FD,
        /* OpFunctionEnd (word_count=1, opcode=56) */
        0x00010038,
    };
    const size_t word_count = sizeof(vertex_spv) / sizeof(uint32_t);

    WgvkSpvModule* module = calloc(1, sizeof(WgvkSpvModule));
    if (!module) { printf("[SKIP] test_simple_vertex_shader (OOM)\n"); return; }

    int result = wgvk_spirv_parse(module, vertex_spv, word_count);
    assert(result == 0 && "parse must succeed");
    assert(module->entry_point_count == 1);
    assert(module->entry_points[0].exec_model == WGVK_SPV_EXEC_MODEL_VERTEX);
    assert(module->function_count == 1);
    assert(module->functions[0]->block_count == 1);
    assert(module->functions[0]->blocks[0].label_id == 4);

    /* Verify the return instruction was captured in the block */
    int found_return = 0;
    WgvkSpvBlock* blk = &module->functions[0]->blocks[0];
    for (uint32_t i = 0; i < blk->instruction_count; i++) {
        if (blk->instructions[i].opcode == WGVK_SPV_OP_RETURN) {
            found_return = 1;
            break;
        }
    }
    assert(found_return && "OpReturn must be recorded in the block");

    /* Verify WGSL generation produces non-empty output */
    WgvkWgslGenerator gen = {0};
    int gen_result = wgvk_wgsl_init(&gen, module, WGVK_SPV_EXEC_MODEL_VERTEX);
    assert(gen_result == 0);
    char* wgsl = wgvk_wgsl_generate(&gen);
    assert(wgsl != NULL && strlen(wgsl) > 0 && "WGSL output must be non-empty");
    free(wgsl);
    wgvk_wgsl_free(&gen);
    wgvk_spirv_free(module);
    free(module);

    printf("[PASS] test_simple_vertex_shader\n");
}

static void test_get_type(void) {
    WgvkSpvModule* module = calloc(1, sizeof(WgvkSpvModule));
    if (!module) { printf("[SKIP] test_get_type (OOM)\n"); return; }
    
    WgvkSpvType* type = wgvk_spirv_get_type(module, 1);
    assert(type == NULL);
    
    printf("[PASS] test_get_type\n");
    free(module);
}

static void test_get_constant(void) {
    WgvkSpvModule* module = calloc(1, sizeof(WgvkSpvModule));
    if (!module) { printf("[SKIP] test_get_constant (OOM)\n"); return; }
    
    WgvkSpvConstant* constant = wgvk_spirv_get_constant(module, 1);
    assert(constant == NULL);
    
    printf("[PASS] test_get_constant\n");
    free(module);
}

static void test_get_variable(void) {
    WgvkSpvModule* module = calloc(1, sizeof(WgvkSpvModule));
    if (!module) { printf("[SKIP] test_get_variable (OOM)\n"); return; }
    
    WgvkSpvVariable* variable = wgvk_spirv_get_variable(module, 1);
    assert(variable == NULL);
    
    printf("[PASS] test_get_variable\n");
    free(module);
}

static void test_opcode_values(void) {
    assert(WGVK_SPV_OP_NOP == 0);
    assert(WGVK_SPV_OP_TYPE_VOID == 19);
    assert(WGVK_SPV_OP_TYPE_BOOL == 20);
    assert(WGVK_SPV_OP_TYPE_INT == 21);
    assert(WGVK_SPV_OP_TYPE_FLOAT == 22);
    assert(WGVK_SPV_OP_TYPE_VECTOR == 23);
    assert(WGVK_SPV_OP_FUNCTION == 54);
    assert(WGVK_SPV_OP_LABEL == 248);
    assert(WGVK_SPV_OP_RETURN == 253);
    
    printf("[PASS] test_opcode_values\n");
}

static void test_storage_class_values(void) {
    assert(WGVK_SPV_STORAGE_CLASS_INPUT == 1);
    assert(WGVK_SPV_STORAGE_CLASS_UNIFORM == 2);
    assert(WGVK_SPV_STORAGE_CLASS_OUTPUT == 3);
    assert(WGVK_SPV_STORAGE_CLASS_FUNCTION == 7);
    
    printf("[PASS] test_storage_class_values\n");
}

static void test_decoration_values(void) {
    assert(WGVK_SPV_DECORATION_BLOCK == 2);
    assert(WGVK_SPV_DECORATION_LOCATION == 30);
    assert(WGVK_SPV_DECORATION_BINDING == 33);
    assert(WGVK_SPV_DECORATION_DESCRIPTOR_SET == 34);
    assert(WGVK_SPV_DECORATION_OFFSET == 35);
    
    printf("[PASS] test_decoration_values\n");
}

static void test_builtin_values(void) {
    assert(WGVK_SPV_BUILTIN_POSITION == 0);
    assert(WGVK_SPV_BUILTIN_VERTEX_INDEX == 44);
    assert(WGVK_SPV_BUILTIN_INSTANCE_INDEX == 45);
    assert(WGVK_SPV_BUILTIN_FRAG_COORD == 15);
    
    printf("[PASS] test_builtin_values\n");
}

static void test_execution_model_values(void) {
    assert(WGVK_SPV_EXEC_MODEL_VERTEX == 0);
    assert(WGVK_SPV_EXEC_MODEL_FRAGMENT == 4);
    assert(WGVK_SPV_EXEC_MODEL_GL_COMPUTE == 5);
    
    printf("[PASS] test_execution_model_values\n");
}

int main(void) {
    printf("=== SPIR-V Parser Tests ===\n\n");
    fflush(stdout);
    
    printf("Running test_magic_number...\n"); fflush(stdout);
    test_magic_number();
    printf("Running test_opcode_values...\n"); fflush(stdout);
    test_opcode_values();
    printf("Running test_storage_class_values...\n"); fflush(stdout);
    test_storage_class_values();
    printf("Running test_decoration_values...\n"); fflush(stdout);
    test_decoration_values();
    printf("Running test_builtin_values...\n"); fflush(stdout);
    test_builtin_values();
    printf("Running test_execution_model_values...\n"); fflush(stdout);
    test_execution_model_values();
    printf("Running test_type_structures...\n"); fflush(stdout);
    test_type_structures();
    printf("Running test_parser_init...\n"); fflush(stdout);
    test_parser_init();
    printf("Running test_get_type...\n"); fflush(stdout);
    test_get_type();
    printf("Running test_get_constant...\n"); fflush(stdout);
    test_get_constant();
    printf("Running test_get_variable...\n"); fflush(stdout);
    test_get_variable();
    printf("Running test_simple_vertex_shader...\n"); fflush(stdout);
    test_simple_vertex_shader();
    
    printf("\n=== All tests passed ===\n");
    return 0;
}
