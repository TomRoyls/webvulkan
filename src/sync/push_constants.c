#include "push_constants.h"
#include <string.h>

void wgvk_push_constants_init(VkDevice device) {
    if (!device) return;
    
    WGPUBufferDescriptor desc = {0};
    desc.nextInChain = NULL;
    desc.label = (WGPUStringView){ .data = "PushConstants", .length = WGPU_STRLEN };
    desc.usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst;
    desc.size = WGVK_PUSH_CONSTANT_SIZE * 1024;
    desc.mappedAtCreation = VK_FALSE;
    
    device->push_constant_buffer = wgpuDeviceCreateBuffer(device->wgpu_device, &desc);
}

void wgvk_push_constants_cleanup(VkDevice device) {
    if (device && device->push_constant_buffer) {
        wgpuBufferRelease(device->push_constant_buffer);
        device->push_constant_buffer = NULL;
    }
}

void vkCmdPushConstants(
    VkCommandBuffer commandBuffer,
    VkPipelineLayout layout,
    uint32_t stageFlags,
    uint32_t offset,
    uint32_t size,
    const void* pValues) {
    
    (void)stageFlags;
    (void)layout;
    
    if (!commandBuffer || !pValues || !commandBuffer->device) return;
    if (!commandBuffer->device->push_constant_buffer) return;
    if (offset + size > WGVK_PUSH_CONSTANT_SIZE) return;
    
    WGPUBuffer buffer = commandBuffer->device->push_constant_buffer;
    uint64_t buffer_offset = commandBuffer->push_constant_offset + offset;
    
    wgpuQueueWriteBuffer(
        commandBuffer->device->wgpu_queue,
        buffer,
        buffer_offset,
        pValues,
        size
    );
}

WGPUBuffer wgvk_get_push_constant_buffer(VkDevice device) {
    return device ? device->push_constant_buffer : NULL;
}

uint32_t wgvk_get_push_constant_offset(VkCommandBuffer cmd) {
    return cmd ? cmd->push_constant_offset : 0;
}
