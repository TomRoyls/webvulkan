#include "webvulkan_internal.h"

static void destroy_pipeline(void* obj) {
    VkPipeline pipeline = (VkPipeline)obj;
    if (pipeline->bind_point == VK_PIPELINE_BIND_POINT_GRAPHICS) {
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

static WGPUVertexFormat vk_format_to_wgpu(uint32_t vk_format) {
    switch (vk_format) {
        case VK_FORMAT_R32_SFLOAT:         return WGPUVertexFormat_Float32;
        case VK_FORMAT_R32G32_SFLOAT:      return WGPUVertexFormat_Float32x2;
        case VK_FORMAT_R32G32B32_SFLOAT:   return WGPUVertexFormat_Float32x3;
        case VK_FORMAT_R32G32B32A32_SFLOAT: return WGPUVertexFormat_Float32x4;
        case VK_FORMAT_R32_SINT:           return WGPUVertexFormat_Sint32;
        case VK_FORMAT_R32G32_SINT:        return WGPUVertexFormat_Sint32x2;
        case VK_FORMAT_R32G32B32_SINT:     return WGPUVertexFormat_Sint32x3;
        case VK_FORMAT_R32G32B32A32_SINT:  return WGPUVertexFormat_Sint32x4;
        case VK_FORMAT_R32_UINT:           return WGPUVertexFormat_Uint32;
        case VK_FORMAT_R32G32_UINT:        return WGPUVertexFormat_Uint32x2;
        case VK_FORMAT_R32G32B32_UINT:     return WGPUVertexFormat_Uint32x3;
        case VK_FORMAT_R32G32B32A32_UINT:  return WGPUVertexFormat_Uint32x4;
        default:                           return WGPUVertexFormat_Float32x3;
    }
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
        pipeline->bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
        pipeline->wgpu_pipeline.render = NULL;
        
        WGPUVertexState vertex_state = {0};
        if (info->pStages && info->stageCount > 0) {
            for (uint32_t s = 0; s < info->stageCount; s++) {
                if (info->pStages[s].stage == VK_SHADER_STAGE_VERTEX_BIT) {
                    vertex_state.module = info->pStages[s].module->wgpu_shader;
                    vertex_state.entryPoint = (WGPUStringView){ .data = info->pStages[s].pName, .length = WGPU_STRLEN };
                }
            }
        }
        
        WGPUVertexBufferLayout* buffer_layouts = NULL;
        uint32_t buffer_count = 0;
        WGPUVertexAttribute* all_attributes = NULL;
        uint32_t attr_count = 0;
        
        if (info->pVertexInputState) {
            const VkPipelineVertexInputStateCreateInfo* vi = info->pVertexInputState;
            buffer_count = vi->vertexBindingDescriptionCount;
            attr_count = vi->vertexAttributeDescriptionCount;
            
            if (buffer_count > 0) {
                buffer_layouts = wgvk_alloc(buffer_count * sizeof(WGPUVertexBufferLayout));
                if (attr_count > 0) {
                    all_attributes = wgvk_alloc(attr_count * sizeof(WGPUVertexAttribute));
                }
                
                for (uint32_t b = 0; b < buffer_count; b++) {
                    const VkVertexInputBindingDescription* binding = &vi->pVertexBindingDescriptions[b];
                    buffer_layouts[b].arrayStride = binding->stride;
                    buffer_layouts[b].stepMode = binding->inputRate ? WGPUVertexStepMode_Instance : WGPUVertexStepMode_Vertex;
                    
                    uint32_t attrs_for_binding = 0;
                    uint32_t attr_base = 0;
                    for (uint32_t a = 0; a < attr_count; a++) {
                        if (vi->pVertexAttributeDescriptions[a].binding == binding->binding) {
                            attrs_for_binding++;
                        }
                    }
                    
                    if (all_attributes) {
                        for (uint32_t a = 0; a < attr_count; a++) {
                            const VkVertexInputAttributeDescription* attr = &vi->pVertexAttributeDescriptions[a];
                            if (attr->binding == binding->binding) {
                                all_attributes[attr_base].format = vk_format_to_wgpu(attr->format);
                                all_attributes[attr_base].offset = attr->offset;
                                all_attributes[attr_base].shaderLocation = attr->location;
                                attr_base++;
                            }
                        }
                        buffer_layouts[b].attributeCount = attrs_for_binding;
                        buffer_layouts[b].attributes = all_attributes + (attr_base - attrs_for_binding);
                    }
                }
                vertex_state.bufferCount = buffer_count;
                vertex_state.buffers = buffer_layouts;
            }
        }
        
        WGPUFragmentState fragment_state = {0};
        WGPUColorTargetState color_targets[8] = {0};
        uint32_t color_target_count = 0;
        
        if (info->pStages && info->stageCount > 1) {
            for (uint32_t s = 0; s < info->stageCount; s++) {
                if (info->pStages[s].stage == VK_SHADER_STAGE_FRAGMENT_BIT) {
                    fragment_state.module = info->pStages[s].module->wgpu_shader;
                    fragment_state.entryPoint = (WGPUStringView){ .data = info->pStages[s].pName, .length = WGPU_STRLEN };
                }
            }
            
            if (info->pColorBlendState && info->pColorBlendState->attachmentCount > 0) {
                for (uint32_t c = 0; c < info->pColorBlendState->attachmentCount && c < 8; c++) {
                    color_targets[c].format = WGPUTextureFormat_BGRA8Unorm;
                    color_targets[c].blend = NULL;
                    color_targets[c].writeMask = WGPUColorWriteMask_All;
                    color_target_count++;
                }
            } else {
                color_targets[0].format = WGPUTextureFormat_BGRA8Unorm;
                color_targets[0].blend = NULL;
                color_targets[0].writeMask = WGPUColorWriteMask_All;
                color_target_count = 1;
            }
            fragment_state.targetCount = color_target_count;
            fragment_state.targets = color_targets;
        }
        
        WGPUPrimitiveState primitive_state = {
            .topology = WGPUPrimitiveTopology_TriangleList,
            .stripIndexFormat = WGPUIndexFormat_Undefined,
            .frontFace = WGPUFrontFace_CW,
            .cullMode = WGPUCullMode_None,
        };
        
        if (info->pInputAssemblyState) {
            switch (info->pInputAssemblyState->topology) {
                case 0: primitive_state.topology = WGPUPrimitiveTopology_PointList; break;
                case 1: primitive_state.topology = WGPUPrimitiveTopology_LineList; break;
                case 2: primitive_state.topology = WGPUPrimitiveTopology_LineStrip; break;
                case 3: primitive_state.topology = WGPUPrimitiveTopology_TriangleList; break;
                case 4: primitive_state.topology = WGPUPrimitiveTopology_TriangleStrip; break;
                default: primitive_state.topology = WGPUPrimitiveTopology_TriangleList; break;
            }
        }
        
        WGPURenderPipelineDescriptor desc = {
            .layout = info->layout ? info->layout->wgpu_layout : NULL,
            .vertex = vertex_state,
            .fragment = color_target_count > 0 ? &fragment_state : NULL,
            .primitive = primitive_state,
            .depthStencil = NULL,
            .multisample = {
                .count = 1,
                .mask = 0xFFFFFFFF,
                .alphaToCoverageEnabled = VK_FALSE,
            },
        };
        
        pipeline->wgpu_pipeline.render = wgpuDeviceCreateRenderPipeline(device->wgpu_device, &desc);
        
        if (buffer_layouts) wgvk_free(buffer_layouts);
        if (all_attributes) wgvk_free(all_attributes);
        
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
        pipeline->bind_point = VK_PIPELINE_BIND_POINT_COMPUTE;
        pipeline->wgpu_pipeline.compute = NULL;
        
        WGPUComputePipelineDescriptor desc = {
            .layout = info->layout ? info->layout->wgpu_layout : NULL,
            .compute = {
                .module = info->stage.module->wgpu_shader,
                .entryPoint = (WGPUStringView){ .data = info->stage.pName, .length = WGPU_STRLEN },
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

WGPURenderPipeline wgvk_pipeline_get_render(VkPipeline pipeline) {
    return pipeline ? pipeline->wgpu_pipeline.render : NULL;
}
