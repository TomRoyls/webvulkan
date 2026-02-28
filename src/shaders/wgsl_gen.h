#ifndef WGVK_WGSL_GEN_H
#define WGVK_WGSL_GEN_H

#include <stddef.h>
#include <stdint.h>
#include "spirv_parser.h"

/**
 * WGSL code generator state.
 *
 * Ownership model:
 *   - The generator struct itself is CALLER-OWNED. Allocate it on the stack
 *     or heap as needed; webvulkan never frees it.
 *   - wgvk_wgsl_init()     initialises internal fields (does not allocate gen).
 *   - wgvk_wgsl_generate() returns a heap-allocated WGSL string owned by the
 *     CALLER. Free it with wgvkFreeWgslOutput() or plain free().
 *   - wgvk_wgsl_free()     releases the internal output buffer inside gen
 *     but does NOT free gen itself.
 *
 * Typical usage (stack-allocated generator):
 *   WgvkWgslGenerator gen = {0};
 *   wgvk_wgsl_init(&gen, module, exec_model);
 *   char* wgsl = wgvk_wgsl_generate(&gen);
 *   wgvk_wgsl_free(&gen);  // release internal buffer
 *   // use wgsl ...
 *   free(wgsl);
 *
 * Heap-allocated generator:
 *   WgvkWgslGenerator* gen = calloc(1, sizeof(*gen));
 *   wgvk_wgsl_init(gen, module, exec_model);
 *   char* wgsl = wgvk_wgsl_generate(gen);
 *   wgvk_wgsl_free(gen);  // release internal buffer
 *   free(gen);             // caller frees the generator itself
 *   free(wgsl);
 */
typedef struct {
	char *buffer;
	size_t cursor;
	size_t capacity;
	WgvkSpvModule *module;
	uint32_t entry_point_id;
	int exec_model;
} WgvkWgslGenerator;

/** Initialise gen for the given module and execution model. Returns 0 on success, -1 on error. */
int wgvk_wgsl_init(WgvkWgslGenerator *gen, WgvkSpvModule *module, uint32_t exec_model);

/**
 * Generate WGSL source from the parsed SPIR-V module.
 * Returns a heap-allocated NUL-terminated string owned by the caller,
 * or NULL on failure. Free with wgvkFreeWgslOutput() or free().
 */
char *wgvk_wgsl_generate(WgvkWgslGenerator *gen);

/**
 * Release the internal output buffer allocated inside gen.
 * Does NOT free gen itself -- that is the caller's responsibility.
 * Safe to call on a zero-initialised or already-freed generator.
 */
void wgvk_wgsl_free(WgvkWgslGenerator *gen);

#endif
