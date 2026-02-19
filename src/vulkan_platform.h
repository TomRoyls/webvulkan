#ifndef VULKAN_PLATFORM_H
#define VULKAN_PLATFORM_H

#define VK_DEFINE_NON_DISPATCHABLE_HANDLE(object) typedef struct object##_T *object;
#define VK_DEFINE_DISPATCHABLE_HANDLE(object) typedef struct object##_T *object;

#endif
