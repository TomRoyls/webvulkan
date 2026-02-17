#ifndef WGVK_WGSL_GEN_H
#define WGVK_WGSL_GEN_H

#include "spirv_parser.h"
#include <stdint.h>
#include <stddef.h>

#define WGVK_WGSL_BUFFER_SIZE 65536

typedef struct {
    char buffer[WGVK_WGSL_BUFFER_SIZE];
    size_t cursor;
    WgvkSpvModule* module;
    uint32_t entry_point_id;
    int exec_model;
} WgvkWgslGenerator;

int wgvk_wgsl_init(WgvkWgslGenerator* gen, WgvkSpvModule* module, uint32_t exec_model);
char* wgvk_wgsl_generate(WgvkWgslGenerator* gen);
void wgvk_wgsl_free(WgvkWgslGenerator* gen);

#endif
