#include "webvulkan_internal.h"

static void destroy_pipeline_layout(void* obj) {
    VkPipelineLayout layout = (VkPipelineLayout)obj;
    
    if (layout->wgpu_layout) {
        wgpuPipelineLayoutRelease(layout->wgpu_layout);
    }
    
    for (uint32_t i = 0; i < layout->set_layout_count; i++) {
        if (layout->set_layouts[i]) {
            wgvk_object_release(&layout->set_layouts[i]->base);
        }
    }
    
    wgvk_free(layout);
}

VkResult vkCreatePipelineLayout(
    VkDevice device,
    const VkPipelineLayoutCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkPipelineLayout* pPipelineLayout) {
    
    (void)pAllocator;
    
    if (!device || !pCreateInfo || !pPipelineLayout) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    
    VkPipelineLayout layout = wgvk_alloc(sizeof(struct VkPipelineLayout_T));
    if (!layout) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    
    wgvk_object_init(&layout->base, destroy_pipeline_layout);
    layout->device = device;
    layout->wgpu_layout = NULL;
    layout->set_layout_count = pCreateInfo->setLayoutCount;
    layout->push_constant_size = 0;
    
    // Store set layouts and calculate push constant size
    const VkPushConstantRange* push_ranges = (const VkPushConstantRange*)pCreateInfo->pPushConstantRanges;
    for (uint32_t i = 0; i < pCreateInfo->pushConstantRangeCount; i++) {
        if (push_ranges[i].offset + push_ranges[i].size > layout->push_constant_size) {
            layout->push_constant_size = push_ranges[i].offset + push_ranges[i].size;
        }
    }
    
    // Cap push constant size
    if (layout->push_constant_size > WGVK_PUSH_CONSTANT_SIZE) {
        layout->push_constant_size = WGVK_PUSH_CONSTANT_SIZE;
    }
    
    // Copy set layouts
    uint32_t count = pCreateInfo->setLayoutCount;
    if (count > WGVK_MAX_BIND_GROUPS) {
        count = WGVK_MAX_BIND_GROUPS;
    }
    
    WGPUBindGroupLayout bind_group_layouts[WGVK_MAX_BIND_GROUPS] = {0};
    
    for (uint32_t i = 0; i < count; i++) {
        layout->set_layouts[i] = pCreateInfo->pSetLayouts[i];
        if (layout->set_layouts[i]) {
            wgvk_object_retain(&layout->set_layouts[i]->base);
            bind_group_layouts[i] = layout->set_layouts[i]->wgpu_layout;
        }
    }
    
    // Create WebGPU pipeline layout
    WGPUPipelineLayoutDescriptor desc = {0};
    desc.nextInChain = NULL;
    desc.label = (WGPUStringView){ .data = "VkPipelineLayout", .length = WGPU_STRLEN };
    desc.bindGroupLayoutCount = count;
    desc.bindGroupLayouts = bind_group_layouts;
    
    layout->wgpu_layout = wgpuDeviceCreatePipelineLayout(device->wgpu_device, &desc);
    
    *pPipelineLayout = layout;
    return VK_SUCCESS;
}

void vkDestroyPipelineLayout(
    VkDevice device,
    VkPipelineLayout pipelineLayout,
    const VkAllocationCallbacks* pAllocator) {
    
    (void)device;
    (void)pAllocator;
    
    if (pipelineLayout) {
        wgvk_object_release(&pipelineLayout->base);
    }
}
