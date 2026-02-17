#include "webvulkan_internal.h"

int wgvk_naga_init(void) {
    return 0;
}

void wgvk_naga_shutdown(void) {
}

char* wgvk_naga_spv_to_wgsl(const uint32_t* spv_code, size_t spv_size) {
    (void)spv_code;
    (void)spv_size;
    return NULL;
}

void wgvk_naga_free_string(char* str) {
    if (str) {
        free(str);
    }
}
