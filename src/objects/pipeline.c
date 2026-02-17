#include "webvulkan_internal.h"

static void destroy_pipeline_layout(void* obj) {
    VkPipelineLayout layout = (VkPipelineLayout)obj;
    if (layout->wgpu_layout) {
        wgpuPipelineLayoutRelease(layout->wgpu_layout);
    }
    free(layout);
}

VkResult vkCreatePipelineLayout(
    VkDevice device,
    const VkPipelineLayoutCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkPipelineLayout* pPipelineLayout)
{
    (void)pAllocator;
    
    if (!device || !pCreateInfo || !pPipelineLayout) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    VkPipelineLayout layout = wgvk_alloc(sizeof(struct VkPipelineLayout_T));
    if (!layout) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    
    wgvk_object_init(&layout->base, destroy_pipeline_layout);
    layout->device = device;
    layout->set_layout_count = pCreateInfo->setLayoutCount;
    layout->push_constant_size = WGVK_PUSH_CONSTANT_SIZE;
    layout->wgpu_layout = NULL;
    
    for (uint32_t i = 0; i < pCreateInfo->setLayoutCount && i < WGVK_MAX_BIND_GROUPS; i++) {
        layout->set_layouts[i] = pCreateInfo->pSetLayouts[i];
    }
    
    WGPUBindGroupLayout bind_group_layouts[WGVK_MAX_BIND_GROUPS] = {0};
    for (uint32_t i = 0; i < layout->set_layout_count && i < WGVK_MAX_BIND_GROUPS; i++) {
        if (layout->set_layouts[i]) {
            bind_group_layouts[i] = layout->set_layouts[i]->wgpu_layout;
        }
    }
    
    WGPUPipelineLayoutDescriptor desc = {
        .bindGroupLayoutCount = layout->set_layout_count,
        .bindGroupLayouts = bind_group_layouts,
    };
    
    layout->wgpu_layout = wgpuDeviceCreatePipelineLayout(device->wgpu_device, &desc);
    if (!layout->wgpu_layout) {
        free(layout);
        return VK_ERROR_OUT_OF_DEVICE_MEMORY;
    }
    
    *pPipelineLayout = layout;
    return VK_SUCCESS;
}

void vkDestroyPipelineLayout(
    VkDevice device,
    VkPipelineLayout pipelineLayout,
    const VkAllocationCallbacks* pAllocator)
{
    (void)device;
    (void)pAllocator;
    if (pipelineLayout) {
        wgvk_object_release(&pipelineLayout->base);
    }
}

static void destroy_pipeline(void* obj) {
    VkPipeline pipeline = (VkPipeline)obj;
    if (pipeline->bind_point == 0) {
        if (pipeline->wgpu_pipeline.render) {
            wgpuRenderPipelineRelease(pipeline->wgpu_pipeline.render);
        }
    } else {
        if (pipeline->wgpu_pipeline.compute) {
            wgpuComputePipelineRelease(pipeline->wgpu_pipeline.compute);
        }
    }
    free(pipeline);
}

VkResult vkCreateGraphicsPipelines(
    VkDevice device,
    VkPipelineCache pipelineCache,
    uint32_t createInfoCount,
    const VkGraphicsPipelineCreateInfo* pCreateInfos,
    const VkAllocationCallbacks* pAllocator,
    VkPipeline* pPipelines)
{
    (void)pipelineCache;
    (void)pAllocator;
    
    if (!device || !pCreateInfos || !pPipelines) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    for (uint32_t i = 0; i < createInfoCount; i++) {
        const VkGraphicsPipelineCreateInfo* info = &pCreateInfos[i];
        
        VkPipeline pipeline = wgvk_alloc(sizeof(struct VkPipeline_T));
        if (!pipeline) {
            for (uint32_t j = 0; j < i; j++) {
                wgvk_object_release(&pPipelines[j]->base);
            }
            return VK_ERROR_OUT_OF_HOST_MEMORY;
        }
        
        wgvk_object_init(&pipeline->base, destroy_pipeline);
        pipeline->device = device;
        pipeline->layout = info->layout;
        pipeline->bind_point = 0;
        pipeline->wgpu_pipeline.render = NULL;
        
        WGPUVertexState vertex_state = {0};
        if (info->pStages && info->stageCount > 0) {
            for (uint32_t s = 0; s < info->stageCount; s++) {
                if (info->pStages[s].stage == 0) {
                    vertex_state.module = info->pStages[s].module->wgpu_shader;
                    vertex_state.entryPoint = info->pStages[s].pName;
                }
            }
        }
        
        WGPUFragmentState fragment_state = {0};
        if (info->pStages && info->stageCount > 1) {
            for (uint32_t s = 0; s < info->stageCount; s++) {
                if (info->pStages[s].stage == 1) {
                    fragment_state.module = info->pStages[s].module->wgpu_shader;
                    fragment_state.entryPoint = info->pStages[s].pName;
                }
            }
        }
        
        WGPURenderPipelineDescriptor desc = {
            .layout = info->layout ? info->layout->wgpu_layout : NULL,
            .vertex = vertex_state,
            .fragment = &fragment_state,
            .primitive = {
                .topology = WGPUPrimitiveTopology_TriangleList,
                .cullMode = WGPUCullMode_None,
            },
        };
        
        pipeline->wgpu_pipeline.render = wgpuDeviceCreateRenderPipeline(device->wgpu_device, &desc);
        if (!pipeline->wgpu_pipeline.render) {
            free(pipeline);
            for (uint32_t j = 0; j < i; j++) {
                wgvk_object_release(&pPipelines[j]->base);
            }
            return VK_ERROR_OUT_OF_DEVICE_MEMORY;
        }
        
        pPipelines[i] = pipeline;
    }
    
    return VK_SUCCESS;
}

VkResult vkCreateComputePipelines(
    VkDevice device,
    VkPipelineCache pipelineCache,
    uint32_t createInfoCount,
    const VkComputePipelineCreateInfo* pCreateInfos,
    const VkAllocationCallbacks* pAllocator,
    VkPipeline* pPipelines)
{
    (void)pipelineCache;
    (void)pAllocator;
    
    if (!device || !pCreateInfos || !pPipelines) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    for (uint32_t i = 0; i < createInfoCount; i++) {
        const VkComputePipelineCreateInfo* info = &pCreateInfos[i];
        
        VkPipeline pipeline = wgvk_alloc(sizeof(struct VkPipeline_T));
        if (!pipeline) {
            for (uint32_t j = 0; j < i; j++) {
                wgvk_object_release(&pPipelines[j]->base);
            }
            return VK_ERROR_OUT_OF_HOST_MEMORY;
        }
        
        wgvk_object_init(&pipeline->base, destroy_pipeline);
        pipeline->device = device;
        pipeline->layout = info->layout;
        pipeline->bind_point = 1;
        pipeline->wgpu_pipeline.compute = NULL;
        
        WGPUComputePipelineDescriptor desc = {
            .layout = info->layout ? info->layout->wgpu_layout : NULL,
            .compute = {
                .module = info->stage.module->wgpu_shader,
                .entryPoint = info->stage.pName,
            },
        };
        
        pipeline->wgpu_pipeline.compute = wgpuDeviceCreateComputePipeline(device->wgpu_device, &desc);
        if (!pipeline->wgpu_pipeline.compute) {
            free(pipeline);
            for (uint32_t j = 0; j < i; j++) {
                wgvk_object_release(&pPipelines[j]->base);
            }
            return VK_ERROR_OUT_OF_DEVICE_MEMORY;
        }
        
        pPipelines[i] = pipeline;
    }
    
    return VK_SUCCESS;
}

void vkDestroyPipeline(
    VkDevice device,
    VkPipeline pipeline,
    const VkAllocationCallbacks* pAllocator)
{
    (void)device;
    (void)pAllocator;
    if (pipeline) {
        wgvk_object_release(&pipeline->base);
    }
}
