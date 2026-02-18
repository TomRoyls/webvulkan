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
    /* Map Vulkan format to WebGPU format */
    /* See: https://www.w3.org/TR/webgpu/#texture-format-caps */
    switch (vk_format) {
        case 37: return WGPUTextureFormat_RGBA8Unorm;      /* VK_FORMAT_R8G8B8A8_UNORM */
        case 38: return WGPUTextureFormat_RGBA8Snorm;      /* VK_FORMAT_R8G8B8A8_SNORM */
        case 39: return WGPUTextureFormat_RGBA8Uint;       /* VK_FORMAT_R8G8B8A8_UINT */
        case 40: return WGPUTextureFormat_RGBA8Sint;       /* VK_FORMAT_R8G8B8A8_SINT */
        case 43: return WGPUTextureFormat_BGRA8Unorm;      /* VK_FORMAT_B8G8R8A8_UNORM */
        case 44: return WGPUTextureFormat_BGRA8UnormSrgb;  /* VK_FORMAT_B8G8R8A8_SRGB */
        case 50: return WGPUTextureFormat_Depth16Unorm;    /* VK_FORMAT_D16_UNORM */
        case 55: return WGPUTextureFormat_Depth24Plus;     /* VK_FORMAT_X8_D24_UNORM_PACK32 */
        case 56: return WGPUTextureFormat_Depth32Float;    /* VK_FORMAT_D32_SFLOAT */
        case 57: return WGPUTextureFormat_Depth24PlusStencil8; /* VK_FORMAT_D24_UNORM_S8_UINT */
        case 58: return WGPUTextureFormat_Depth32FloatStencil8; /* VK_FORMAT_D32_SFLOAT_S8_UINT */
        case 64: return WGPUTextureFormat_BC1RGBAUnorm;    /* VK_FORMAT_BC1_RGBA_UNORM_BLOCK */
        case 65: return WGPUTextureFormat_BC1RGBAUnormSrgb; /* VK_FORMAT_BC1_RGBA_SRGB_BLOCK */
        case 66: return WGPUTextureFormat_BC2RGBAUnorm;    /* VK_FORMAT_BC2_RGBA_UNORM_BLOCK */
        case 67: return WGPUTextureFormat_BC2RGBAUnormSrgb; /* VK_FORMAT_BC2_RGBA_SRGB_BLOCK */
        case 68: return WGPUTextureFormat_BC3RGBAUnorm;    /* VK_FORMAT_BC3_RGBA_UNORM_BLOCK */
        case 69: return WGPUTextureFormat_BC3RGBAUnormSrgb; /* VK_FORMAT_BC3_RGBA_SRGB_BLOCK */
        case 70: return WGPUTextureFormat_BC4RUnorm;       /* VK_FORMAT_BC4_R_UNORM_BLOCK */
        case 71: return WGPUTextureFormat_BC4RSnorm;       /* VK_FORMAT_BC4_R_SNORM_BLOCK */
        case 72: return WGPUTextureFormat_BC5RGUnorm;      /* VK_FORMAT_BC5_RG_UNORM_BLOCK */
        case 73: return WGPUTextureFormat_BC5RGSnorm;      /* VK_FORMAT_BC5_RG_SNORM_BLOCK */
        case 77: return WGPUTextureFormat_BC7RGBAUnorm;    /* VK_FORMAT_BC7_RGBA_UNORM_BLOCK */
        case 78: return WGPUTextureFormat_BC7RGBAUnormSrgb; /* VK_FORMAT_BC7_RGBA_SRGB_BLOCK */
        case 91: return WGPUTextureFormat_R8Unorm;         /* VK_FORMAT_R8_UNORM */
        case 92: return WGPUTextureFormat_R8Snorm;         /* VK_FORMAT_R8_SNORM */
        case 93: return WGPUTextureFormat_R8Uint;          /* VK_FORMAT_R8_UINT */
        case 94: return WGPUTextureFormat_R8Sint;          /* VK_FORMAT_R8_SINT */
        case 95: return WGPUTextureFormat_R16Uint;         /* VK_FORMAT_R16_UINT */
        case 96: return WGPUTextureFormat_R16Sint;         /* VK_FORMAT_R16_SINT */
        case 97: return WGPUTextureFormat_R16Float;        /* VK_FORMAT_R16_SFLOAT */
        case 98: return WGPUTextureFormat_RG8Unorm;        /* VK_FORMAT_R8G8_UNORM */
        case 99: return WGPUTextureFormat_RG8Snorm;        /* VK_FORMAT_R8G8_SNORM */
        case 100: return WGPUTextureFormat_RG8Uint;        /* VK_FORMAT_R8G8_UINT */
        case 101: return WGPUTextureFormat_RG8Sint;        /* VK_FORMAT_R8G8_SINT */
        case 103: return WGPUTextureFormat_R32Float;       /* VK_FORMAT_R32_SFLOAT */
        case 104: return WGPUTextureFormat_R32Uint;        /* VK_FORMAT_R32_UINT */
        case 105: return WGPUTextureFormat_R32Sint;        /* VK_FORMAT_R32_SINT */
        case 106: return WGPUTextureFormat_RG16Uint;       /* VK_FORMAT_R16G16_UINT */
        case 107: return WGPUTextureFormat_RG16Sint;       /* VK_FORMAT_R16G16_SINT */
        case 108: return WGPUTextureFormat_RG16Float;      /* VK_FORMAT_R16G16_SFLOAT */
        case 109: return WGPUTextureFormat_RGBA16Uint;     /* VK_FORMAT_R16G16B16A16_UINT */
        case 110: return WGPUTextureFormat_RGBA16Sint;     /* VK_FORMAT_R16G16B16A16_SINT */
        case 111: return WGPUTextureFormat_RGBA16Float;    /* VK_FORMAT_R16G16B16A16_SFLOAT */
        case 114: return WGPUTextureFormat_RGBA32Float;    /* VK_FORMAT_R32G32B32A32_SFLOAT */
        case 115: return WGPUTextureFormat_RGBA32Uint;     /* VK_FORMAT_R32G32B32A32_UINT */
        case 116: return WGPUTextureFormat_RGBA32Sint;     /* VK_FORMAT_R32G32B32A32_SINT */
        case 122: return WGPUTextureFormat_RG32Float;      /* VK_FORMAT_R32G32_SFLOAT */
        case 123: return WGPUTextureFormat_RG32Uint;       /* VK_FORMAT_R32G32_UINT */
        case 124: return WGPUTextureFormat_RG32Sint;       /* VK_FORMAT_R32G32_SINT */
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
    
    /* Map Vulkan usage flags to WebGPU usage flags */
    /* Vulkan: TRANSFER_SRC=1, TRANSFER_DST=2, SAMPLED=4, STORAGE=8, COLOR_ATTACH=16, DEPTH_STENCIL=32 */
    /* WebGPU: CopySrc=1, CopyDst=2, TextureBinding=4, StorageBinding=8, RenderAttachment=16 */
    WGPUTextureUsage wgpu_usage = WGPUTextureUsage_None;
    if (pCreateInfo->usage & 0x00000001) wgpu_usage |= WGPUTextureUsage_CopySrc;
    if (pCreateInfo->usage & 0x00000002) wgpu_usage |= WGPUTextureUsage_CopyDst;
    if (pCreateInfo->usage & 0x00000004) wgpu_usage |= WGPUTextureUsage_TextureBinding;
    if (pCreateInfo->usage & 0x00000008) wgpu_usage |= WGPUTextureUsage_StorageBinding;
    if (pCreateInfo->usage & 0x00000010) wgpu_usage |= WGPUTextureUsage_RenderAttachment;
    if (pCreateInfo->usage & 0x00000020) wgpu_usage |= WGPUTextureUsage_RenderAttachment; /* depth/stencil */
    
    WGPUExtent3D size = {
        .width = pCreateInfo->extent[0],
        .height = pCreateInfo->extent[1] > 0 ? pCreateInfo->extent[1] : 1,
        .depthOrArrayLayers = pCreateInfo->extent[2] > 0 ? pCreateInfo->extent[2] : pCreateInfo->arrayLayers,
    };
    
    WGPUTextureDescriptor desc = {
        .nextInChain = NULL,
        .label = WGPU_STRING_VIEW_INIT,
        .usage = wgpu_usage,
        .dimension = image_type_to_dimension(pCreateInfo->imageType),
        .size = size,
        .format = vulkan_format_to_webgpu(pCreateInfo->format),
        .mipLevelCount = pCreateInfo->mipLevels,
        .sampleCount = pCreateInfo->samples,
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
