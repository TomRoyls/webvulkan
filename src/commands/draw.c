#include "webvulkan_internal.h"

void vkCmdBindPipeline(
    VkCommandBuffer commandBuffer,
    uint32_t pipelineBindPoint,
    VkPipeline pipeline)
{
    if (!commandBuffer || !pipeline) {
        return;
    }
    
    commandBuffer->bound_pipeline = pipeline;
    commandBuffer->bound_layout = pipeline->layout;
    
    if (pipelineBindPoint == 0) {
        if (commandBuffer->wgpu_render_pass) {
            wgpuRenderPassEncoderSetPipeline(
                commandBuffer->wgpu_render_pass,
                pipeline->wgpu_pipeline.render);
        }
    } else {
        if (commandBuffer->wgpu_compute_pass) {
            wgpuComputePassEncoderSetPipeline(
                commandBuffer->wgpu_compute_pass,
                pipeline->wgpu_pipeline.compute);
        }
    }
}

void vkCmdBindVertexBuffers(
    VkCommandBuffer commandBuffer,
    uint32_t firstBinding,
    uint32_t bindingCount,
    const VkBuffer* pBuffers,
    const VkDeviceSize* pOffsets)
{
    if (!commandBuffer || !pBuffers || !pOffsets) {
        return;
    }
    
    for (uint32_t i = 0; i < bindingCount; i++) {
        uint32_t slot = firstBinding + i;
        if (slot < WGVK_MAX_VERTEX_BUFFERS) {
            commandBuffer->bound_vertex_buffers[slot] = pBuffers[i];
            commandBuffer->bound_vertex_offsets[slot] = pOffsets[i];
            
            if (commandBuffer->wgpu_render_pass && pBuffers[i]) {
                wgpuRenderPassEncoderSetVertexBuffer(
                    commandBuffer->wgpu_render_pass,
                    slot,
                    pBuffers[i]->wgpu_buffer,
                    pOffsets[i]);
            }
        }
    }
}

void vkCmdBindIndexBuffer(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    uint32_t indexType)
{
    if (!commandBuffer || !buffer) {
        return;
    }
    
    commandBuffer->bound_index_buffer = buffer;
    commandBuffer->bound_index_offset = offset;
    commandBuffer->bound_index_type = indexType;
    
    if (commandBuffer->wgpu_render_pass) {
        WGPUIndexFormat format = WGPUIndexFormat_Uint32;
        if (indexType == 0) {
            format = WGPUIndexFormat_Uint16;
        } else if (indexType == 1) {
            format = WGPUIndexFormat_Uint32;
        }
        
        wgpuRenderPassEncoderSetIndexBuffer(
            commandBuffer->wgpu_render_pass,
            buffer->wgpu_buffer,
            format,
            offset,
            buffer->size - offset);
    }
}

void vkCmdBindDescriptorSets(
    VkCommandBuffer commandBuffer,
    uint32_t pipelineBindPoint,
    VkPipelineLayout layout,
    uint32_t firstSet,
    uint32_t descriptorSetCount,
    const VkDescriptorSet* pDescriptorSets,
    uint32_t dynamicOffsetCount,
    const uint32_t* pDynamicOffsets)
{
    (void)layout;
    (void)dynamicOffsetCount;
    (void)pDynamicOffsets;
    
    if (!commandBuffer || !pDescriptorSets) {
        return;
    }
    
    for (uint32_t i = 0; i < descriptorSetCount; i++) {
        uint32_t slot = firstSet + i;
        if (slot < WGVK_MAX_BIND_GROUPS) {
            commandBuffer->bound_descriptor_sets[slot] = pDescriptorSets[i];
            
            if (pipelineBindPoint == 0 && commandBuffer->wgpu_render_pass) {
                if (pDescriptorSets[i] && pDescriptorSets[i]->wgpu_bind_group) {
                    wgpuRenderPassEncoderSetBindGroup(
                        commandBuffer->wgpu_render_pass,
                        slot,
                        pDescriptorSets[i]->wgpu_bind_group,
                        0,
                        NULL);
                }
            } else if (pipelineBindPoint == 1 && commandBuffer->wgpu_compute_pass) {
                if (pDescriptorSets[i] && pDescriptorSets[i]->wgpu_bind_group) {
                    wgpuComputePassEncoderSetBindGroup(
                        commandBuffer->wgpu_compute_pass,
                        slot,
                        pDescriptorSets[i]->wgpu_bind_group,
                        0,
                        NULL);
                }
            }
        }
    }
}

void vkCmdDraw(
    VkCommandBuffer commandBuffer,
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance)
{
    if (!commandBuffer || !commandBuffer->wgpu_render_pass) {
        return;
    }
    
    wgpuRenderPassEncoderDraw(
        commandBuffer->wgpu_render_pass,
        vertexCount,
        instanceCount,
        firstVertex,
        firstInstance);
}

void vkCmdDrawIndexed(
    VkCommandBuffer commandBuffer,
    uint32_t indexCount,
    uint32_t instanceCount,
    uint32_t firstIndex,
    int32_t vertexOffset,
    uint32_t firstInstance)
{
    if (!commandBuffer || !commandBuffer->wgpu_render_pass) {
        return;
    }
    
    wgpuRenderPassEncoderDrawIndexed(
        commandBuffer->wgpu_render_pass,
        indexCount,
        instanceCount,
        firstIndex,
        vertexOffset,
        firstInstance);
}

void vkCmdDrawIndirect(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    uint32_t drawCount,
    uint32_t stride)
{
    (void)stride;
    
    if (!commandBuffer || !buffer || !commandBuffer->wgpu_render_pass) {
        return;
    }
    
    for (uint32_t i = 0; i < drawCount; i++) {
        wgpuRenderPassEncoderDrawIndirect(
            commandBuffer->wgpu_render_pass,
            buffer->wgpu_buffer,
            offset + i * stride);
    }
}

void vkCmdDrawIndexedIndirect(
    VkCommandBuffer commandBuffer,
    VkBuffer buffer,
    VkDeviceSize offset,
    uint32_t drawCount,
    uint32_t stride)
{
    (void)stride;
    
    if (!commandBuffer || !buffer || !commandBuffer->wgpu_render_pass) {
        return;
    }
    
    for (uint32_t i = 0; i < drawCount; i++) {
        wgpuRenderPassEncoderDrawIndexedIndirect(
            commandBuffer->wgpu_render_pass,
            buffer->wgpu_buffer,
            offset + i * stride);
    }
}
