#include "webvulkan_internal.h"

int wgvk_spirv_parse(const uint32_t* code, size_t size, void* out_module) {
    (void)code;
    (void)size;
    (void)out_module;
    return 0;
}

void wgvk_spirv_free(void* module) {
    (void)module;
}

const char* wgvk_spirv_get_entry_point(void* module, uint32_t stage) {
    (void)module;
    (void)stage;
    return "main";
}
