#ifndef VULKAN_PLATFORM_H
#define VULKAN_PLATFORM_H

/*
 * WebVulkan platform configuration for Vulkan headers.
 * 
 * We need Vulkan handles to be pointers (not uint64_t) so we can
 * dereference them as our internal structs. This header must be
 * included BEFORE vulkan_core.h.
 */

/* Prevent default handle definitions */
#define VK_DEFINE_NON_DISPATCHABLE_HANDLE(object) typedef struct object##_T *object;
#define VK_DEFINE_DISPATCHABLE_HANDLE(object) typedef struct object##_T *object;

/* Now include Vulkan */
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan_core.h>

#endif /* VULKAN_PLATFORM_H */
