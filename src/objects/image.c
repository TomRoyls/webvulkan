#include "webvulkan_internal.h"

static void destroy_image(void* obj) {
    VkImage image = (VkImage)obj;
    if (image->wgpu_texture) {
        wgpuTextureRelease(image->wgpu_texture);
    }
    free(image);
}

static WGPUTextureDimension image_type_to_dimension(uint32_t image_type) {
    switch (image_type) {
        case 0: return WGPUTextureDimension_1D;
        case 1: return WGPUTextureDimension_2D;
        case 2: return WGPUTextureDimension_3D;
        default: return WGPUTextureDimension_2D;
    }
}

static WGPUTextureFormat vulkan_format_to_webgpu(uint32_t vk_format) {
    switch (vk_format) {
        case 37: return WGPUTextureFormat_RGBA8Unorm;
        case 38: return WGPUTextureFormat_RGBA8Snorm;
        case 39: return WGPUTextureFormat_RGBA8Uint;
        case 40: return WGPUTextureFormat_RGBA8Sint;
        case 43: return WGPUTextureFormat_BGRA8Unorm;
        case 44: return WGPUTextureFormat_BGRA8UnormSrgb;
        case 50: return WGPUTextureFormat_Depth16Unorm;
        case 55: return WGPUTextureFormat_Depth24Plus;
        case 56: return WGPUTextureFormat_Depth32Float;
        case 57: return WGPUTextureFormat_Depth24PlusStencil8;
        case 58: return WGPUTextureFormat_Depth32FloatStencil8;
        case 64: return WGPUTextureFormat_BC1RGBAUnorm;
        case 65: return WGPUTextureFormat_BC1RGBAUnormSrgb;
        case 66: return WGPUTextureFormat_BC2RGBAUnorm;
        case 67: return WGPUTextureFormat_BC2RGBAUnormSrgb;
        case 68: return WGPUTextureFormat_BC3RGBAUnorm;
        case 69: return WGPUTextureFormat_BC3RGBAUnormSrgb;
        case 70: return WGPUTextureFormat_BC4RUnorm;
        case 71: return WGPUTextureFormat_BC4RSnorm;
        case 72: return WGPUTextureFormat_BC5RGUnorm;
        case 73: return WGPUTextureFormat_BC5RGSnorm;
        case 77: return WGPUTextureFormat_BC7RGBAUnorm;
        case 78: return WGPUTextureFormat_BC7RGBAUnormSrgb;
        case 91: return WGPUTextureFormat_R8Unorm;
        case 92: return WGPUTextureFormat_R8Snorm;
        case 93: return WGPUTextureFormat_R8Uint;
        case 94: return WGPUTextureFormat_R8Sint;
        case 95: return WGPUTextureFormat_R16Uint;
        case 96: return WGPUTextureFormat_R16Sint;
        case 97: return WGPUTextureFormat_R16Float;
        case 98: return WGPUTextureFormat_RG8Unorm;
        case 99: return WGPUTextureFormat_RG8Snorm;
        case 100: return WGPUTextureFormat_RG8Uint;
        case 101: return WGPUTextureFormat_RG8Sint;
        case 103: return WGPUTextureFormat_R32Float;
        case 104: return WGPUTextureFormat_R32Uint;
        case 105: return WGPUTextureFormat_R32Sint;
        case 106: return WGPUTextureFormat_RG16Uint;
        case 107: return WGPUTextureFormat_RG16Sint;
        case 108: return WGPUTextureFormat_RG16Float;
        case 109: return WGPUTextureFormat_RGBA16Uint;
        case 110: return WGPUTextureFormat_RGBA16Sint;
        case 111: return WGPUTextureFormat_RGBA16Float;
        case 114: return WGPUTextureFormat_RGBA32Float;
        case 115: return WGPUTextureFormat_RGBA32Uint;
        case 116: return WGPUTextureFormat_RGBA32Sint;
        case 122: return WGPUTextureFormat_RG32Float;
        case 123: return WGPUTextureFormat_RG32Uint;
        case 124: return WGPUTextureFormat_RG32Sint;
        default: return WGPUTextureFormat_RGBA8Unorm;
    }
}

VkResult vkCreateImage(
    VkDevice device,
    const VkImageCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkImage* pImage)
{
    (void)pAllocator;
    
    if (!device || !pCreateInfo || !pImage) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    VkImage image = wgvk_alloc(sizeof(struct VkImage_T));
    if (!image) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    
    wgvk_object_init(&image->base, destroy_image);
    image->device = device;
    image->width = pCreateInfo->extent[0];
    image->height = pCreateInfo->extent[1];
    image->depth = pCreateInfo->extent[2];
    image->mip_levels = pCreateInfo->mipLevels;
    image->array_layers = pCreateInfo->arrayLayers;
    image->format = pCreateInfo->format;
    image->image_type = pCreateInfo->imageType;
    image->samples = pCreateInfo->samples;
    image->usage = pCreateInfo->usage;
    image->bound_memory = NULL;
    image->memory_offset = 0;
    image->wgpu_texture = NULL;
    
    WGPUTextureUsage wgpu_usage = WGPUTextureUsage_None;
    if (pCreateInfo->usage & 0x00000001) wgpu_usage |= WGPUTextureUsage_Sampled;
    if (pCreateInfo->usage & 0x00000002) wgpu_usage |= WGPUTextureUsage_Storage;
    if (pCreateInfo->usage & 0x00000004) wgpu_usage |= WGPUTextureUsage_ColorAttachment;
    if (pCreateInfo->usage & 0x00000008) wgpu_usage |= WGPUTextureUsage_DepthStencilAttachment;
    if (pCreateInfo->usage & 0x00001000) wgpu_usage |= WGPUTextureUsage_CopySrc;
    if (pCreateInfo->usage & 0x00002000) wgpu_usage |= WGPUTextureUsage_CopyDst;
    if (pCreateInfo->usage & 0x00000080) wgpu_usage |= WGPUTextureUsage_RenderAttachment;
    
    WGPUTextureDescriptor desc = {
        .dimension = image_type_to_dimension(pCreateInfo->imageType),
        .size = {
            .width = pCreateInfo->extent[0],
            .height = pCreateInfo->extent[1] > 0 ? pCreateInfo->extent[1] : 1,
            .depthOrArrayLayers = pCreateInfo->extent[2] > 0 ? pCreateInfo->extent[2] : pCreateInfo->arrayLayers,
        },
        .format = vulkan_format_to_webgpu(pCreateInfo->format),
        .mipLevelCount = pCreateInfo->mipLevels,
        .sampleCount = pCreateInfo->samples,
        .usage = wgpu_usage,
    };
    
    image->wgpu_texture = wgpuDeviceCreateTexture(device->wgpu_device, &desc);
    if (!image->wgpu_texture) {
        free(image);
        return VK_ERROR_OUT_OF_DEVICE_MEMORY;
    }
    
    *pImage = image;
    return VK_SUCCESS;
}

void vkDestroyImage(
    VkDevice device,
    VkImage image,
    const VkAllocationCallbacks* pAllocator)
{
    (void)device;
    (void)pAllocator;
    if (image) {
        wgvk_object_release(&image->base);
    }
}

static void destroy_image_view(void* obj) {
    VkImageView view = (VkImageView)obj;
    if (view->wgpu_view) {
        wgpuTextureViewRelease(view->wgpu_view);
    }
    free(view);
}

VkResult vkCreateImageView(
    VkDevice device,
    const VkImageViewCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkImageView* pView)
{
    (void)pAllocator;
    
    if (!device || !pCreateInfo || !pView) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    VkImageView view = wgvk_alloc(sizeof(struct VkImageView_T));
    if (!view) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    
    wgvk_object_init(&view->base, destroy_image_view);
    view->device = device;
    view->image = pCreateInfo->image;
    view->view_type = pCreateInfo->viewType;
    view->format = pCreateInfo->format;
    view->wgpu_view = NULL;
    
    WGPUTextureViewDescriptor desc = {
        .format = vulkan_format_to_webgpu(pCreateInfo->format),
        .dimension = WGPUTextureViewDimension_2D,
        .baseMipLevel = 0,
        .mipLevelCount = pCreateInfo->image->mip_levels,
        .baseArrayLayer = 0,
        .arrayLayerCount = 1,
    };
    
    view->wgpu_view = wgpuTextureCreateView(pCreateInfo->image->wgpu_texture, &desc);
    if (!view->wgpu_view) {
        free(view);
        return VK_ERROR_OUT_OF_DEVICE_MEMORY;
    }
    
    *pView = view;
    return VK_SUCCESS;
}

void vkDestroyImageView(
    VkDevice device,
    VkImageView imageView,
    const VkAllocationCallbacks* pAllocator)
{
    (void)device;
    (void)pAllocator;
    if (imageView) {
        wgvk_object_release(&imageView->base);
    }
}
