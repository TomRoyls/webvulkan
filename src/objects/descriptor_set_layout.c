#include "webvulkan_internal.h"

typedef struct VkDescriptorSetLayoutBinding {
    uint32_t binding;
    uint32_t descriptorType;
    uint32_t descriptorCount;
    uint32_t stageFlags;
    const void* pImmutableSamplers;
} VkDescriptorSetLayoutBinding;

typedef struct VkDescriptorSetLayoutCreateInfo {
    VkStructureType sType;
    const void* pNext;
    VkFlags flags;
    uint32_t bindingCount;
    const VkDescriptorSetLayoutBinding* pBindings;
} VkDescriptorSetLayoutCreateInfo;

static void destroy_descriptor_set_layout(void* obj) {
    VkDescriptorSetLayout layout = (VkDescriptorSetLayout)obj;
    
    if (layout->wgpu_layout) {
        wgpuBindGroupLayoutRelease(layout->wgpu_layout);
    }
    
    wgvk_free(layout);
}

VkResult vkCreateDescriptorSetLayout(
    VkDevice device,
    const VkDescriptorSetLayoutCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDescriptorSetLayout* pSetLayout) {
    
    (void)pAllocator;
    
    if (!device || !pCreateInfo || !pSetLayout) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    
    VkDescriptorSetLayout layout = wgvk_alloc(sizeof(struct VkDescriptorSetLayout_T));
    if (!layout) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    
    wgvk_object_init(&layout->base, destroy_descriptor_set_layout);
    layout->device = device;
    layout->wgpu_layout = NULL;
    layout->binding_count = pCreateInfo->bindingCount;
    
    // Build WebGPU bind group layout entries
    WGPUBindGroupLayoutEntry entries[32] = {0};
    uint32_t entry_count = 0;
    
    for (uint32_t i = 0; i < pCreateInfo->bindingCount && entry_count < 32; i++) {
        const VkDescriptorSetLayoutBinding* binding = &pCreateInfo->pBindings[i];
        WGPUBindGroupLayoutEntry* entry = &entries[entry_count];
        
        entry->binding = binding->binding;
        entry->visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment | WGPUShaderStage_Compute;
        
        switch (binding->descriptorType) {
            case 0: // VK_DESCRIPTOR_TYPE_SAMPLER
                entry->sampler.type = WGPUSamplerBindingType_Filtering;
                entry_count++;
                break;
            case 1: // VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
                entry->texture.sampleType = WGPUTextureSampleType_Float;
                entry->texture.viewDimension = WGPUTextureViewDimension_2D;
                entry->sampler.type = WGPUSamplerBindingType_Filtering;
                entry_count++;
                break;
            case 2: // VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE
                entry->texture.sampleType = WGPUTextureSampleType_Float;
                entry->texture.viewDimension = WGPUTextureViewDimension_2D;
                entry_count++;
                break;
            case 3: // VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
                entry->storageTexture.access = WGPUStorageTextureAccess_WriteOnly;
                entry->storageTexture.format = WGPUTextureFormat_RGBA8Unorm;
                entry->storageTexture.viewDimension = WGPUTextureViewDimension_2D;
                entry_count++;
                break;
            case 4: // VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER
                entry->texture.sampleType = WGPUTextureSampleType_Float;
                entry->texture.viewDimension = WGPUTextureViewDimension_1D;
                entry_count++;
                break;
            case 5: // VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER
                entry->storageTexture.access = WGPUStorageTextureAccess_WriteOnly;
                entry->storageTexture.format = WGPUTextureFormat_RGBA8Unorm;
                entry->storageTexture.viewDimension = WGPUTextureViewDimension_1D;
                entry_count++;
                break;
            case 6: // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
            case 7: // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC
                entry->buffer.type = WGPUBufferBindingType_Uniform;
                entry->buffer.hasDynamicOffset = (binding->descriptorType == 7);
                entry_count++;
                break;
            case 7: // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
            case 8: // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC
                entry->buffer.type = WGPUBufferBindingType_Storage;
                entry->buffer.hasDynamicOffset = (binding->descriptorType == 8);
                entry_count++;
                break;
            case 10: // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
                entry->buffer.type = WGPUBufferBindingType_Uniform;
                entry->buffer.hasDynamicOffset = true;
                entry_count++;
                break;
            default:
                break;
        }
    }
    
    // Create WebGPU bind group layout
    WGPUBindGroupLayoutDescriptor desc = {0};
    desc.nextInChain = NULL;
    desc.label = "VkDescriptorSetLayout";
    desc.entryCount = entry_count;
    desc.entries = entries;
    
    layout->wgpu_layout = wgpuDeviceCreateBindGroupLayout(device->wgpu_device, &desc);
    
    *pSetLayout = layout;
    return VK_SUCCESS;
}

void vkDestroyDescriptorSetLayout(
    VkDevice device,
    VkDescriptorSetLayout descriptorSetLayout,
    const VkAllocationCallbacks* pAllocator) {
    
    (void)device;
    (void)pAllocator;
    
    if (descriptorSetLayout) {
        wgvk_object_release(&descriptorSetLayout->base);
    }
}
