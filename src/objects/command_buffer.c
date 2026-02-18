#include "webvulkan_internal.h"

static void destroy_command_buffer(void* obj) {
    VkCommandBuffer cmd = (VkCommandBuffer)obj;
    if (cmd->wgpu_encoder) {
        wgpuCommandEncoderRelease(cmd->wgpu_encoder);
    }
    free(cmd);
}

VkResult vkAllocateCommandBuffers(
    VkDevice device,
    const VkCommandBufferAllocateInfo* pAllocateInfo,
    VkCommandBuffer* pCommandBuffers)
{
    if (!device || !pAllocateInfo || !pCommandBuffers) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    for (uint32_t i = 0; i < pAllocateInfo->commandBufferCount; i++) {
        VkCommandBuffer cmd = wgvk_alloc(sizeof(struct VkCommandBuffer_T));
        if (!cmd) {
            for (uint32_t j = 0; j < i; j++) {
                wgvk_object_release(&pCommandBuffers[j]->base);
            }
            return VK_ERROR_OUT_OF_HOST_MEMORY;
        }
        
        wgvk_object_init(&cmd->base, destroy_command_buffer);
        cmd->device = device;
        cmd->pool = pAllocateInfo->commandPool;
        cmd->wgpu_encoder = NULL;
        cmd->wgpu_render_pass = NULL;
        cmd->wgpu_compute_pass = NULL;
        cmd->level = pAllocateInfo->level;
        cmd->recording = false;
        cmd->in_render_pass = false;
        cmd->in_compute_pass = false;
        cmd->bound_pipeline = NULL;
        cmd->bound_layout = NULL;
        cmd->bound_index_buffer = NULL;
        cmd->bound_index_offset = 0;
        cmd->bound_index_type = 0;
        
        for (int j = 0; j < WGVK_MAX_VERTEX_BUFFERS; j++) {
            cmd->bound_vertex_buffers[j] = NULL;
            cmd->bound_vertex_offsets[j] = 0;
        }
        for (int j = 0; j < WGVK_MAX_BIND_GROUPS; j++) {
            cmd->bound_descriptor_sets[j] = NULL;
        }
        
        pCommandBuffers[i] = cmd;
    }
    
    return VK_SUCCESS;
}

void vkFreeCommandBuffers(
    VkDevice device,
    VkCommandPool commandPool,
    uint32_t commandBufferCount,
    const VkCommandBuffer* pCommandBuffers)
{
    (void)device;
    (void)commandPool;
    
    for (uint32_t i = 0; i < commandBufferCount; i++) {
        if (pCommandBuffers[i]) {
            wgvk_object_release(&pCommandBuffers[i]->base);
        }
    }
}

VkResult vkBeginCommandBuffer(
    VkCommandBuffer commandBuffer,
    const VkCommandBufferBeginInfo* pBeginInfo)
{
    (void)pBeginInfo;
    
    if (!commandBuffer) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    if (commandBuffer->recording) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    WGPUCommandEncoderDescriptor desc = {};
    commandBuffer->wgpu_encoder = wgpuDeviceCreateCommandEncoder(
        commandBuffer->device->wgpu_device, &desc);
    
    if (!commandBuffer->wgpu_encoder) {
        return VK_ERROR_OUT_OF_DEVICE_MEMORY;
    }
    
    commandBuffer->recording = true;
    commandBuffer->in_render_pass = false;
    commandBuffer->in_compute_pass = false;
    
    return VK_SUCCESS;
}

VkResult vkEndCommandBuffer(
    VkCommandBuffer commandBuffer)
{
    if (!commandBuffer || !commandBuffer->recording) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    if (commandBuffer->in_render_pass) {
        wgpuRenderPassEncoderEnd(commandBuffer->wgpu_render_pass);
        wgpuRenderPassEncoderRelease(commandBuffer->wgpu_render_pass);
        commandBuffer->wgpu_render_pass = NULL;
        commandBuffer->in_render_pass = false;
    }
    
    if (commandBuffer->in_compute_pass) {
        wgpuComputePassEncoderEnd(commandBuffer->wgpu_compute_pass);
        wgpuComputePassEncoderRelease(commandBuffer->wgpu_compute_pass);
        commandBuffer->wgpu_compute_pass = NULL;
        commandBuffer->in_compute_pass = false;
    }
    
    commandBuffer->recording = false;
    return VK_SUCCESS;
}
