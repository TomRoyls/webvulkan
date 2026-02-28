#ifndef WGVK_PUSH_CONSTANTS_H
#define WGVK_PUSH_CONSTANTS_H

#include "webvulkan_internal.h"

void wgvk_push_constants_init(VkDevice device);
void wgvk_push_constants_cleanup(VkDevice device);

void vkCmdPushConstants(VkCommandBuffer commandBuffer, VkPipelineLayout layout, uint32_t stageFlags,
                        uint32_t offset, uint32_t size, const void *pValues);

WGPUBuffer wgvk_get_push_constant_buffer(VkDevice device);
uint32_t wgvk_get_push_constant_offset(VkCommandBuffer cmd);

#endif
