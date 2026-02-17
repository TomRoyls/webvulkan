#include "webvulkan_internal.h"

static void destroy_descriptor_set_layout(void* obj) {
    VkDescriptorSetLayout layout = (VkDescriptorSetLayout)obj;
    if (layout->wgpu_layout) {
        wgpuBindGroupLayoutRelease(layout->wgpu_layout);
    }
    free(layout);
}

VkResult vkCreateDescriptorSetLayout(
    VkDevice device,
    const void* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDescriptorSetLayout* pSetLayout)
{
    (void)pAllocator;
    
    if (!device || !pCreateInfo || !pSetLayout) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    const struct {
        VkStructureType sType;
        const void* pNext;
        VkFlags flags;
        uint32_t bindingCount;
        const void* pBindings;
    }* create_info = pCreateInfo;
    
    VkDescriptorSetLayout layout = wgvk_alloc(sizeof(struct VkDescriptorSetLayout_T));
    if (!layout) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    
    wgvk_object_init(&layout->base, destroy_descriptor_set_layout);
    layout->device = device;
    layout->wgpu_layout = NULL;
    layout->binding_count = create_info->bindingCount;
    
    WGPUBindGroupLayoutEntry* entries = NULL;
    if (create_info->bindingCount > 0) {
        entries = wgvk_alloc(create_info->bindingCount * sizeof(WGPUBindGroupLayoutEntry));
        if (!entries) {
            free(layout);
            return VK_ERROR_OUT_OF_HOST_MEMORY;
        }
        memset(entries, 0, create_info->bindingCount * sizeof(WGPUBindGroupLayoutEntry));
        
        const struct {
            uint32_t binding;
            uint32_t descriptorType;
            uint32_t descriptorCount;
            uint32_t stageFlags;
            const void* pImmutableSamplers;
        }* bindings = create_info->pBindings;
        
        for (uint32_t i = 0; i < create_info->bindingCount; i++) {
            entries[i].binding = bindings[i].binding;
            entries[i].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment | WGPUShaderStage_Compute;
            
            switch (bindings[i].descriptorType) {
                case 0: // VK_DESCRIPTOR_TYPE_SAMPLER
                    entries[i].sampler.type = WGPUSamplerBindingType_Filtering;
                    break;
                case 1: // VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
                    entries[i].sampler.type = WGPUSamplerBindingType_Filtering;
                    entries[i].texture.sampleType = WGPUTextureSampleType_Float;
                    entries[i].texture.viewDimension = WGPUTextureViewDimension_2D;
                    break;
                case 2: // VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE
                    entries[i].texture.sampleType = WGPUTextureSampleType_Float;
                    entries[i].texture.viewDimension = WGPUTextureViewDimension_2D;
                    break;
                case 3: // VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
                    entries[i].storageTexture.access = WGPUStorageTextureAccess_WriteOnly;
                    entries[i].storageTexture.format = WGPUTextureFormat_RGBA8Unorm;
                    entries[i].storageTexture.viewDimension = WGPUTextureViewDimension_2D;
                    break;
                case 6: // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
                    entries[i].buffer.type = WGPUBufferBindingType_Uniform;
                    entries[i].buffer.hasDynamicOffset = false;
                    break;
                case 7: // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
                    entries[i].buffer.type = WGPUBufferBindingType_Storage;
                    entries[i].buffer.hasDynamicOffset = false;
                    break;
                case 8: // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
                    entries[i].buffer.type = WGPUBufferBindingType_Uniform;
                    entries[i].buffer.hasDynamicOffset = true;
                    break;
                case 9: // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC
                    entries[i].buffer.type = WGPUBufferBindingType_Storage;
                    entries[i].buffer.hasDynamicOffset = true;
                    break;
                default:
                    entries[i].buffer.type = WGPUBufferBindingType_Uniform;
                    break;
            }
        }
    }
    
    WGPUBindGroupLayoutDescriptor desc = {
        .entryCount = create_info->bindingCount,
        .entries = entries,
    };
    
    layout->wgpu_layout = wgpuDeviceCreateBindGroupLayout(device->wgpu_device, &desc);
    
    if (entries) {
        free(entries);
    }
    
    if (!layout->wgpu_layout) {
        free(layout);
        return VK_ERROR_OUT_OF_DEVICE_MEMORY;
    }
    
    *pSetLayout = layout;
    return VK_SUCCESS;
}

void vkDestroyDescriptorSetLayout(
    VkDevice device,
    VkDescriptorSetLayout descriptorSetLayout,
    const VkAllocationCallbacks* pAllocator)
{
    (void)device;
    (void)pAllocator;
    if (descriptorSetLayout) {
        wgvk_object_release(&descriptorSetLayout->base);
    }
}

static void destroy_descriptor_pool(void* obj) {
    VkDescriptorPool pool = (VkDescriptorPool)obj;
    free(pool);
}

VkResult vkCreateDescriptorPool(
    VkDevice device,
    const void* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDescriptorPool* pDescriptorPool)
{
    (void)pCreateInfo;
    (void)pAllocator;
    
    if (!device || !pDescriptorPool) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    VkDescriptorPool pool = wgvk_alloc(sizeof(struct VkDescriptorPool_T));
    if (!pool) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    
    wgvk_object_init(&pool->base, destroy_descriptor_pool);
    pool->device = device;
    
    *pDescriptorPool = pool;
    return VK_SUCCESS;
}

void vkDestroyDescriptorPool(
    VkDevice device,
    VkDescriptorPool descriptorPool,
    const VkAllocationCallbacks* pAllocator)
{
    (void)device;
    (void)pAllocator;
    if (descriptorPool) {
        wgvk_object_release(&descriptorPool->base);
    }
}

static void destroy_descriptor_set(void* obj) {
    VkDescriptorSet set = (VkDescriptorSet)obj;
    if (set->wgpu_bind_group) {
        wgpuBindGroupRelease(set->wgpu_bind_group);
    }
    free(set);
}

VkResult vkAllocateDescriptorSets(
    VkDevice device,
    const void* pAllocateInfo,
    VkDescriptorSet* pDescriptorSets)
{
    if (!device || !pAllocateInfo || !pDescriptorSets) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    const struct {
        VkStructureType sType;
        const void* pNext;
        VkDescriptorPool descriptorPool;
        uint32_t descriptorSetCount;
        const VkDescriptorSetLayout* pSetLayouts;
    }* alloc_info = pAllocateInfo;
    
    for (uint32_t i = 0; i < alloc_info->descriptorSetCount; i++) {
        VkDescriptorSet set = wgvk_alloc(sizeof(struct VkDescriptorSet_T));
        if (!set) {
            for (uint32_t j = 0; j < i; j++) {
                wgvk_object_release(&pDescriptorSets[j]->base);
            }
            return VK_ERROR_OUT_OF_HOST_MEMORY;
        }
        
        wgvk_object_init(&set->base, destroy_descriptor_set);
        set->device = device;
        set->layout = alloc_info->pSetLayouts[i];
        set->wgpu_bind_group = NULL;
        
        if (set->layout && set->layout->wgpu_layout) {
            WGPUBindGroupDescriptor desc = {
                .layout = set->layout->wgpu_layout,
                .entryCount = 0,
                .entries = NULL,
            };
            
            set->wgpu_bind_group = wgpuDeviceCreateBindGroup(device->wgpu_device, &desc);
        }
        
        pDescriptorSets[i] = set;
    }
    
    return VK_SUCCESS;
}

VkResult vkFreeDescriptorSets(
    VkDevice device,
    VkDescriptorPool descriptorPool,
    uint32_t descriptorSetCount,
    const VkDescriptorSet* pDescriptorSets)
{
    (void)device;
    (void)descriptorPool;
    
    for (uint32_t i = 0; i < descriptorSetCount; i++) {
        if (pDescriptorSets[i]) {
            wgvk_object_release(&pDescriptorSets[i]->base);
        }
    }
    
    return VK_SUCCESS;
}

void vkUpdateDescriptorSets(
    VkDevice device,
    uint32_t descriptorWriteCount,
    const void* pDescriptorWrites,
    uint32_t descriptorCopyCount,
    const void* pDescriptorCopies)
{
    (void)device;
    (void)descriptorCopyCount;
    (void)pDescriptorCopies;
    
    if (!pDescriptorWrites || descriptorWriteCount == 0) {
        return;
    }
    
    const struct {
        VkStructureType sType;
        const void* pNext;
        VkDescriptorSet dstSet;
        uint32_t dstBinding;
        uint32_t dstArrayElement;
        uint32_t descriptorCount;
        uint32_t descriptorType;
        union {
            const void* pBufferInfo;
            const void* pImageInfo;
            const void* pTexelBufferView;
        };
    }* writes = pDescriptorWrites;
    
    for (uint32_t i = 0; i < descriptorWriteCount; i++) {
        VkDescriptorSet set = writes[i].dstSet;
        if (!set) continue;
        
        (void)writes[i].descriptorType;
        (void)writes[i].descriptorCount;
    }
}
