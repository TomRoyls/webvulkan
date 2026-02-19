#include "webvulkan_internal.h"

static void destroy_device_memory(void* obj) {
    VkDeviceMemory mem = (VkDeviceMemory)obj;
    if (mem->wgpu_buffer) {
        wgpuBufferRelease(mem->wgpu_buffer);
    }
    free(mem);
}

VkResult vkAllocateMemory(
    VkDevice device,
    const void* pAllocateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDeviceMemory* pMemory)
{
    (void)pAllocateInfo;
    (void)pAllocator;
    
    if (!device || !pMemory) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    const struct {
        VkStructureType sType;
        const void* pNext;
        VkDeviceSize allocationSize;
        uint32_t memoryTypeIndex;
    }* alloc_info = pAllocateInfo;
    
    VkDeviceMemory mem = wgvk_alloc(sizeof(struct VkDeviceMemory_T));
    if (!mem) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    
    wgvk_object_init(&mem->base, destroy_device_memory);
    mem->device = device;
    mem->size = alloc_info->allocationSize;
    mem->memory_type_index = alloc_info->memoryTypeIndex;
    mem->mapped_ptr = NULL;
    mem->wgpu_buffer = NULL;
    
    WGPUBufferDescriptor desc = {
        .size = alloc_info->allocationSize,
        .usage = WGPUBufferUsage_CopySrc | WGPUBufferUsage_CopyDst | 
                 WGPUBufferUsage_Uniform | WGPUBufferUsage_Storage |
                 WGPUBufferUsage_Vertex | WGPUBufferUsage_Index,
        .mappedAtCreation = VK_FALSE,
    };
    
    mem->wgpu_buffer = wgpuDeviceCreateBuffer(device->wgpu_device, &desc);
    
    if (!mem->wgpu_buffer) {
        free(mem);
        return VK_ERROR_OUT_OF_DEVICE_MEMORY;
    }
    
    *pMemory = mem;
    return VK_SUCCESS;
}

void vkFreeMemory(
    VkDevice device,
    VkDeviceMemory memory,
    const VkAllocationCallbacks* pAllocator)
{
    (void)device;
    (void)pAllocator;
    if (memory) {
        wgvk_object_release(&memory->base);
    }
}

VkResult vkMapMemory(
    VkDevice device,
    VkDeviceMemory memory,
    VkDeviceSize offset,
    VkDeviceSize size,
    VkFlags flags,
    void** ppData)
{
    (void)device;
    (void)offset;
    (void)size;
    (void)flags;
    
    if (!memory || !ppData) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    if (!memory->wgpu_buffer) {
        return VK_ERROR_MEMORY_MAP_FAILED;
    }
    
    memory->mapped_ptr = wgpuBufferGetMappedRange(memory->wgpu_buffer, 0, size);
    
    if (!memory->mapped_ptr) {
        return VK_ERROR_MEMORY_MAP_FAILED;
    }
    
    *ppData = memory->mapped_ptr;
    return VK_SUCCESS;
}

void vkUnmapMemory(
    VkDevice device,
    VkDeviceMemory memory)
{
    (void)device;
    
    if (!memory || !memory->wgpu_buffer) {
        return;
    }
    
    wgpuBufferUnmap(memory->wgpu_buffer);
    memory->mapped_ptr = NULL;
}

VkResult vkBindBufferMemory(
    VkDevice device,
    VkBuffer buffer,
    VkDeviceMemory memory,
    VkDeviceSize memoryOffset)
{
    (void)device;
    (void)memory;
    (void)memoryOffset;
    
    if (!buffer) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    buffer->bound_memory = memory;
    buffer->memory_offset = memoryOffset;
    
    return VK_SUCCESS;
}

VkResult vkBindImageMemory(
    VkDevice device,
    VkImage image,
    VkDeviceMemory memory,
    VkDeviceSize memoryOffset)
{
    (void)device;
    (void)memory;
    (void)memoryOffset;
    
    if (!image) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    image->bound_memory = memory;
    image->memory_offset = memoryOffset;
    
    return VK_SUCCESS;
}

void vkGetBufferMemoryRequirements(
    VkDevice device,
    VkBuffer buffer,
    void* pMemoryRequirements)
{
    (void)device;
    (void)buffer;
    
    if (!pMemoryRequirements) {
        return;
    }
    
    struct {
        VkDeviceSize size;
        VkDeviceSize alignment;
        uint32_t memoryTypeBits;
    }* reqs = pMemoryRequirements;
    
    reqs->size = buffer ? buffer->size : 0;
    reqs->alignment = 256;
    reqs->memoryTypeBits = 0x1;
}

void vkGetImageMemoryRequirements(
    VkDevice device,
    VkImage image,
    void* pMemoryRequirements)
{
    (void)device;
    (void)image;
    
    if (!pMemoryRequirements) {
        return;
    }
    
    struct {
        VkDeviceSize size;
        VkDeviceSize alignment;
        uint32_t memoryTypeBits;
    }* reqs = pMemoryRequirements;
    
    reqs->size = 1024 * 1024;
    reqs->alignment = 256;
    reqs->memoryTypeBits = 0x1;
}
