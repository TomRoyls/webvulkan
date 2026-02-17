#include "webvulkan_internal.h"

void* wgvk_allocator_alloc(size_t size, size_t alignment) {
    (void)alignment;
    return malloc(size);
}

void wgvk_allocator_free(void* ptr) {
    free(ptr);
}
