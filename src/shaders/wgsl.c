#include "webvulkan_internal.h"

char* wgvk_wgsl_from_spirv(const uint32_t* spirv_code, size_t spirv_size) {
    (void)spirv_code;
    (void)spirv_size;
    return NULL;
}

void wgvk_wgsl_free(char* wgsl) {
    if (wgsl) {
        free(wgsl);
    }
}
