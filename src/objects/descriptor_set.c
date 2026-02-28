#include "webvulkan_internal.h"

static void destroy_descriptor_pool(void* obj) {
    VkDescriptorPool pool = (VkDescriptorPool)obj;
    wgvk_free(pool);
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
    wgvk_free(set);
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
        set->entry_count = 0;
        
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
        const void* pImageInfo;
        const void* pBufferInfo;
        const void* pTexelBufferView;
    }* writes = pDescriptorWrites;
    
    const struct VkDescriptorBufferInfo_T {
        VkBuffer buffer;
        VkDeviceSize offset;
        VkDeviceSize range;
    }* buf_info;
    
    const struct VkDescriptorImageInfo_T {
        VkSampler sampler;
        VkImageView imageView;
        uint32_t imageLayout;
    }* img_info;
    
    for (uint32_t i = 0; i < descriptorWriteCount; i++) {
        VkDescriptorSet set = writes[i].dstSet;
        if (!set || !set->layout) continue;
        
        uint32_t binding = writes[i].dstBinding;
        uint32_t desc_type = writes[i].descriptorType;
        
        for (uint32_t d = 0; d < writes[i].descriptorCount; d++) {
            if (set->entry_count >= 32) break;
            
            WGPUBindGroupEntry* entry = &set->entries[set->entry_count];
            memset(entry, 0, sizeof(*entry));
            entry->binding = binding + d;
            
            if (desc_type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ||
                desc_type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER ||
                desc_type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC ||
                desc_type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC) {
                buf_info = (const struct VkDescriptorBufferInfo_T*)writes[i].pBufferInfo + d;
                if (buf_info && buf_info->buffer && buf_info->buffer->wgpu_buffer) {
                    entry->buffer = buf_info->buffer->wgpu_buffer;
                    entry->offset = buf_info->offset;
                    entry->size   = (buf_info->range == (VkDeviceSize)-1)
                                    ? buf_info->buffer->size - buf_info->offset
                                    : buf_info->range;
                    set->entry_count++;
                }
            } else if (desc_type == VK_DESCRIPTOR_TYPE_SAMPLER) {
                img_info = (const struct VkDescriptorImageInfo_T*)writes[i].pImageInfo + d;
                if (img_info && img_info->sampler && img_info->sampler->wgpu_sampler) {
                    entry->sampler = img_info->sampler->wgpu_sampler;
                    set->entry_count++;
                }
            } else if (desc_type == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE || desc_type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
                img_info = (const struct VkDescriptorImageInfo_T*)writes[i].pImageInfo + d;
                if (img_info && img_info->imageView && img_info->imageView->wgpu_view) {
                    entry->textureView = img_info->imageView->wgpu_view;
                    set->entry_count++;
                }
            }
        }
        
        if (set->wgpu_bind_group) {
            wgpuBindGroupRelease(set->wgpu_bind_group);
            set->wgpu_bind_group = NULL;
        }
        
        if (set->layout->wgpu_layout) {
            WGPUBindGroupDescriptor desc = {
                .layout     = set->layout->wgpu_layout,
                .entryCount = set->entry_count,
                .entries    = set->entries,
            };
            set->wgpu_bind_group = wgpuDeviceCreateBindGroup(device->wgpu_device, &desc);
        }
    }
}
