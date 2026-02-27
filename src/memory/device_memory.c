#include "webvulkan_internal.h"

static void destroy_device_memory(void* obj) {
    VkDeviceMemory mem = (VkDeviceMemory)obj;
    if (mem->wgpu_buffer) {
        wgpuBufferRelease(mem->wgpu_buffer);
    }
    free(mem);
}

static uint32_t vk_format_bytes_per_pixel(uint32_t format) {
    /* Covers the most common VkFormat values.  Unknown formats fall back to 4. */
    switch (format) {
        case 9:  case 10: case 11: case 12:   /* R8_* */
            return 1;
        case 13: case 14: case 15: case 16:  /* R8G8_* */
            return 2;
        case 37: case 38: case 39: case 40:  /* R8G8B8_* */
            return 3;
        case 41: case 42: case 43: case 44:  /* R8G8B8A8_* */
        case 50: case 51: case 52: case 53:  /* B8G8R8A8_* */
            return 4;
        case 70: case 71: case 72: case 73:  /* R16_* */
            return 2;
        case 77: case 78: case 79: case 80:  /* R16G16_* */
            return 4;
        case 83: case 84: case 85: case 86:  /* R16G16B16_* */
            return 6;
        case 87: case 88: case 89: case 90:  /* R16G16B16A16_* */
            return 8;
        case 98:  case 99:  case 100: case 101:  /* R32_{U,S}INT / R32_SFLOAT */
            return 4;
        case 103: case 104: case 105:            /* R32G32_* */
            return 8;
        case 106: case 107: case 108:            /* R32G32B32_* */
            return 12;
        case 109: case 110: case 111: case 112: case 113: /* R32G32B32A32_* */
            return 16;
        case 124:  /* D16_UNORM */
            return 2;
        case 125:  /* X8_D24_UNORM_PACK32 */
        case 126:  /* D32_SFLOAT */
        case 127:  /* S8_UINT */
            return 4;
        case 129:  /* D24_UNORM_S8_UINT */
        case 130:  /* D32_SFLOAT_S8_UINT */
            return 5;
        default:
            return 4;
    }
}

VkResult vkAllocateMemory(
    VkDevice device,
    const VkMemoryAllocateInfo* pAllocateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDeviceMemory* pMemory)
{
    (void)pAllocator;
    
    if (!device || !pMemory) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    VkDeviceMemory mem = wgvk_alloc(sizeof(struct VkDeviceMemory_T));
    if (!mem) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    
    wgvk_object_init(&mem->base, destroy_device_memory);
    mem->device = device;
    mem->size = pAllocateInfo->allocationSize;
    mem->memory_type_index = pAllocateInfo->memoryTypeIndex;
    mem->mapped_ptr = NULL;
    mem->wgpu_buffer = NULL;
    
    WGPUBufferDescriptor desc = {
        .size = pAllocateInfo->allocationSize,
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
    (void)flags;
    (void)size;  /* We map the full allocation; offset is applied to the returned pointer */
    
    if (!memory || !ppData) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    if (!memory->wgpu_buffer) {
        return VK_ERROR_MEMORY_MAP_FAILED;
    }
    
    if (memory->mapped_ptr) {
        /* Already mapped — return existing pointer */
        *ppData = (uint8_t*)memory->mapped_ptr + offset;
        return VK_SUCCESS;
    }
    
    /* Allocate a host-side shadow buffer.  On vkUnmapMemory we upload via
     * wgpuQueueWriteBuffer.  This is the only portable approach across both
     * Emscripten (no synchronous map) and native Dawn. */
    memory->mapped_ptr = malloc((size_t)memory->size);
    if (!memory->mapped_ptr) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    
    *ppData = (uint8_t*)memory->mapped_ptr + offset;
    return VK_SUCCESS;
}

void vkUnmapMemory(
    VkDevice device,
    VkDeviceMemory memory)
{
    if (!memory || !memory->wgpu_buffer || !memory->mapped_ptr) {
        return;
    }
    
    /* Upload the shadow buffer to the GPU buffer, then free the host copy. */
    wgpuQueueWriteBuffer(
        device->wgpu_queue,
        memory->wgpu_buffer,
        0,
        memory->mapped_ptr,
        (size_t)memory->size);
    
    free(memory->mapped_ptr);
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
    
    if (!pMemoryRequirements) {
        return;
    }
    
    struct {
        VkDeviceSize size;
        VkDeviceSize alignment;
        uint32_t memoryTypeBits;
    }* reqs = pMemoryRequirements;
    
    if (!image) {
        reqs->size = 0;
        reqs->alignment = 256;
        reqs->memoryTypeBits = 0x1;
        return;
    }
    
    /* Sum all mip levels: each level halves width and height. */
    uint32_t bpp    = vk_format_bytes_per_pixel(image->format);
    uint32_t depth  = (image->depth       > 0) ? image->depth       : 1;
    uint32_t layers = (image->array_layers > 0) ? image->array_layers : 1;
    uint32_t mips   = (image->mip_levels  > 0) ? image->mip_levels  : 1;
    uint32_t w      = (image->width       > 0) ? image->width       : 1;
    uint32_t h      = (image->height      > 0) ? image->height      : 1;
    
    VkDeviceSize total = 0;
    for (uint32_t m = 0; m < mips; m++) {
        uint32_t mw = (w >> m) > 0 ? (w >> m) : 1;
        uint32_t mh = (h >> m) > 0 ? (h >> m) : 1;
        total += (VkDeviceSize)mw * mh * depth * layers * bpp;
    }
    /* Round up to 256-byte alignment. */
    total = (total + 255) & ~(VkDeviceSize)255;
    
    reqs->size           = total;
    reqs->alignment      = 256;
    reqs->memoryTypeBits = 0x1;
}
