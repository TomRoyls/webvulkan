#include "webvulkan_internal.h"

static void destroy_buffer(void* obj) {
    VkBuffer buffer = (VkBuffer)obj;
    if (buffer->wgpu_buffer) {
        wgpuBufferRelease(buffer->wgpu_buffer);
    }
    free(buffer);
}

VkResult vkCreateBuffer(
    VkDevice device,
    const VkBufferCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkBuffer* pBuffer)
{
    (void)pAllocator;
    
    if (!device || !pCreateInfo || !pBuffer) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    VkBuffer buffer = wgvk_alloc(sizeof(struct VkBuffer_T));
    if (!buffer) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    
    wgvk_object_init(&buffer->base, destroy_buffer);
    buffer->device = device;
    buffer->size = pCreateInfo->size;
    buffer->usage = pCreateInfo->usage;
    buffer->bound_memory = NULL;
    buffer->memory_offset = 0;
    buffer->wgpu_buffer = NULL;
    
    WGPUBufferUsage wgpu_usage = WGPUBufferUsage_None;
    if (pCreateInfo->usage & 0x00000001) wgpu_usage |= WGPUBufferUsage_Vertex;
    if (pCreateInfo->usage & 0x00000002) wgpu_usage |= WGPUBufferUsage_Index;
    if (pCreateInfo->usage & 0x00000004) wgpu_usage |= WGPUBufferUsage_Uniform;
    if (pCreateInfo->usage & 0x00000008) wgpu_usage |= WGPUBufferUsage_Storage;
    if (pCreateInfo->usage & 0x00001000) wgpu_usage |= WGPUBufferUsage_CopySrc;
    if (pCreateInfo->usage & 0x00002000) wgpu_usage |= WGPUBufferUsage_CopyDst;
    if (pCreateInfo->usage & 0x00000010) wgpu_usage |= WGPUBufferUsage_Indirect;
    
    WGPUBufferDescriptor desc = {
        .size = pCreateInfo->size,
        .usage = wgpu_usage,
        .mappedAtCreation = false,
    };
    
    buffer->wgpu_buffer = wgpuDeviceCreateBuffer(device->wgpu_device, &desc);
    if (!buffer->wgpu_buffer) {
        free(buffer);
        return VK_ERROR_OUT_OF_DEVICE_MEMORY;
    }
    
    *pBuffer = buffer;
    return VK_SUCCESS;
}

void vkDestroyBuffer(
    VkDevice device,
    VkBuffer buffer,
    const VkAllocationCallbacks* pAllocator)
{
    (void)device;
    (void)pAllocator;
    if (buffer) {
        wgvk_object_release(&buffer->base);
    }
}
