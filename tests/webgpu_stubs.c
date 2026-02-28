#include <stddef.h>
#include <stdint.h>
#include <webgpu/webgpu.h>

WGPUInstance wgpuCreateInstance(const WGPUInstanceDescriptor *descriptor) {
	(void)descriptor;
	return (WGPUInstance)(uintptr_t)1;
}
void wgpuInstanceRelease(WGPUInstance instance) {
	(void)instance;
}
void wgpuAdapterRelease(WGPUAdapter adapter) {
	(void)adapter;
}
WGPUDevice wgpuAdapterRequestDeviceSync(WGPUAdapter adapter, const WGPUDeviceDescriptor *descriptor) {
	(void)adapter;
	(void)descriptor;
	return (WGPUDevice)(uintptr_t)1;
}
void wgpuDeviceRelease(WGPUDevice device) {
	(void)device;
}
WGPUQueue wgpuDeviceGetQueue(WGPUDevice device) {
	(void)device;
	return (WGPUQueue)(uintptr_t)1;
}
void wgpuQueueRelease(WGPUQueue queue) {
	(void)queue;
}
void wgpuQueueAddRef(WGPUQueue queue) {
	(void)queue;
}
void wgpuQueueSubmit(WGPUQueue queue, size_t commandCount, const WGPUCommandBuffer *commands) {
	(void)queue;
	(void)commandCount;
	(void)commands;
}
void wgpuQueueWriteBuffer(WGPUQueue queue, WGPUBuffer buffer, uint64_t offset, const void *data,
                          size_t size) {
	(void)queue;
	(void)buffer;
	(void)offset;
	(void)data;
	(void)size;
}
WGPUBuffer wgpuDeviceCreateBuffer(WGPUDevice device, const WGPUBufferDescriptor *descriptor) {
	(void)device;
	(void)descriptor;
	return (WGPUBuffer)(uintptr_t)1;
}
void wgpuBufferRelease(WGPUBuffer buffer) {
	(void)buffer;
}
WGPUTexture wgpuDeviceCreateTexture(WGPUDevice device, const WGPUTextureDescriptor *descriptor) {
	(void)device;
	(void)descriptor;
	return (WGPUTexture)(uintptr_t)1;
}
void wgpuTextureRelease(WGPUTexture texture) {
	(void)texture;
}
WGPUTextureView wgpuTextureCreateView(WGPUTexture texture, const WGPUTextureViewDescriptor *descriptor) {
	(void)texture;
	(void)descriptor;
	return (WGPUTextureView)(uintptr_t)1;
}
void wgpuTextureViewRelease(WGPUTextureView view) {
	(void)view;
}
WGPUTextureFormat wgpuTextureGetFormat(WGPUTexture texture) {
	(void)texture;
    return WGPUTextureFormat_Undefined;
}
WGPUShaderModule wgpuDeviceCreateShaderModule(WGPUDevice device, const WGPUShaderModuleDescriptor *descriptor) {
	(void)device;
	(void)descriptor;
	return (WGPUShaderModule)(uintptr_t)1;
}
void wgpuShaderModuleRelease(WGPUShaderModule module) {
	(void)module;
}
WGPURenderPipeline wgpuDeviceCreateRenderPipeline(WGPUDevice device, const WGPURenderPipelineDescriptor *descriptor) {
	(void)device;
	(void)descriptor;
	return (WGPURenderPipeline)(uintptr_t)1;
}
void wgpuRenderPipelineRelease(WGPURenderPipeline pipeline) {
	(void)pipeline;
}
WGPUComputePipeline wgpuDeviceCreateComputePipeline(WGPUDevice device, const WGPUComputePipelineDescriptor *descriptor) {
	(void)device;
	(void)descriptor;
	return (WGPUComputePipeline)(uintptr_t)1;
}
void wgpuComputePipelineRelease(WGPUComputePipeline pipeline) {
	(void)pipeline;
}
WGPUPipelineLayout wgpuDeviceCreatePipelineLayout(WGPUDevice device, const WGPUPipelineLayoutDescriptor *descriptor) {
	(void)device;
	(void)descriptor;
	return (WGPUPipelineLayout)(uintptr_t)1;
}
void wgpuPipelineLayoutRelease(WGPUPipelineLayout layout) {
	(void)layout;
}
WGPUBindGroupLayout wgpuDeviceCreateBindGroupLayout(WGPUDevice device, const WGPUBindGroupLayoutDescriptor *descriptor) {
	(void)device;
	(void)descriptor;
	return (WGPUBindGroupLayout)(uintptr_t)1;
}
void wgpuBindGroupLayoutRelease(WGPUBindGroupLayout layout) {
	(void)layout;
}
WGPUBindGroup wgpuDeviceCreateBindGroup(WGPUDevice device, const WGPUBindGroupDescriptor *descriptor) {
	(void)device;
	(void)descriptor;
	return (WGPUBindGroup)(uintptr_t)1;
}
void wgpuBindGroupRelease(WGPUBindGroup group) {
	(void)group;
}
WGPUSampler wgpuDeviceCreateSampler(WGPUDevice device, const WGPUSamplerDescriptor *descriptor) {
	(void)device;
	(void)descriptor;
	return (WGPUSampler)(uintptr_t)1;
}
void wgpuSamplerRelease(WGPUSampler sampler) {
	(void)sampler;
}
WGPUCommandEncoder wgpuDeviceCreateCommandEncoder(WGPUDevice device, const WGPUCommandEncoderDescriptor *descriptor) {
	(void)device;
	(void)descriptor;
	return (WGPUCommandEncoder)(uintptr_t)1;
}
void wgpuCommandEncoderRelease(WGPUCommandEncoder encoder) {
	(void)encoder;
}
WGPUCommandBuffer wgpuCommandEncoderFinish(WGPUCommandEncoder encoder, const WGPUCommandBufferDescriptor *descriptor) {
	(void)encoder;
	(void)descriptor;
	return (WGPUCommandBuffer)(uintptr_t)1;
}
void wgpuCommandBufferRelease(WGPUCommandBuffer buffer) {
	(void)buffer;
}
void wgpuCommandEncoderCopyBufferToBuffer(WGPUCommandEncoder encoder, WGPUBuffer src,
                                          uint64_t srcOffset, WGPUBuffer dst, uint64_t dstOffset,
                                          uint64_t size) {
	(void)encoder;
	(void)src;
	(void)srcOffset;
	(void)dst;
	(void)dstOffset;
	(void)size;
}
void wgpuCommandEncoderCopyBufferToTexture(WGPUCommandEncoder encoder,
                                           const WGPUTexelCopyBufferInfo *src,
                                           const WGPUTexelCopyTextureInfo *dst,
                                           const WGPUExtent3D *size) {
	(void)encoder;
	(void)src;
	(void)dst;
	(void)size;
}
void wgpuCommandEncoderCopyTextureToBuffer(WGPUCommandEncoder encoder,
                                           const WGPUTexelCopyTextureInfo *src,
                                           const WGPUTexelCopyBufferInfo *dst,
                                           const WGPUExtent3D *size) {
	(void)encoder;
	(void)src;
	(void)dst;
	(void)size;
}
WGPURenderPassEncoder wgpuCommandEncoderBeginRenderPass(WGPUCommandEncoder encoder,
                                                        const WGPURenderPassDescriptor *descriptor) {
	(void)encoder;
	(void)descriptor;
	return (WGPURenderPassEncoder)(uintptr_t)1;
}
void wgpuRenderPassEncoderEnd(WGPURenderPassEncoder encoder) {
	(void)encoder;
}
void wgpuRenderPassEncoderRelease(WGPURenderPassEncoder encoder) {
	(void)encoder;
}
void wgpuRenderPassEncoderSetPipeline(WGPURenderPassEncoder encoder, WGPURenderPipeline pipeline) {
	(void)encoder;
	(void)pipeline;
}
void wgpuRenderPassEncoderSetVertexBuffer(WGPURenderPassEncoder encoder, uint32_t slot,
                                          WGPUBuffer buffer, uint64_t offset, uint64_t size) {
	(void)encoder;
	(void)slot;
	(void)buffer;
	(void)offset;
	(void)size;
}
void wgpuRenderPassEncoderSetIndexBuffer(WGPURenderPassEncoder encoder, WGPUBuffer buffer,
                                         WGPUIndexFormat format, uint64_t offset, uint64_t size) {
	(void)encoder;
	(void)buffer;
	(void)format;
	(void)offset;
	(void)size;
}
void wgpuRenderPassEncoderSetBindGroup(WGPURenderPassEncoder encoder, uint32_t groupIndex,
                                       WGPUBindGroup group, size_t dynamicOffsetCount,
                                       const uint32_t *dynamicOffsets) {
	(void)encoder;
	(void)groupIndex;
	(void)group;
	(void)dynamicOffsetCount;
	(void)dynamicOffsets;
}
void wgpuRenderPassEncoderDraw(WGPURenderPassEncoder encoder, uint32_t vertexCount,
                               uint32_t instanceCount, uint32_t firstVertex,
                               uint32_t firstInstance) {
	(void)encoder;
	(void)vertexCount;
	(void)instanceCount;
	(void)firstVertex;
	(void)firstInstance;
}
void wgpuRenderPassEncoderDrawIndexed(WGPURenderPassEncoder encoder, uint32_t indexCount,
                                      uint32_t instanceCount, uint32_t firstIndex,
                                      int32_t baseVertex, uint32_t firstInstance) {
	(void)encoder;
	(void)indexCount;
	(void)instanceCount;
	(void)firstIndex;
	(void)baseVertex;
	(void)firstInstance;
}
void wgpuRenderPassEncoderDrawIndirect(WGPURenderPassEncoder encoder, WGPUBuffer buffer,
                                       uint64_t offset) {
	(void)encoder;
	(void)buffer;
	(void)offset;
}
void wgpuRenderPassEncoderDrawIndexedIndirect(WGPURenderPassEncoder encoder, WGPUBuffer buffer,
                                              uint64_t offset) {
	(void)encoder;
	(void)buffer;
	(void)offset;
}
void wgpuRenderPassEncoderSetViewport(WGPURenderPassEncoder encoder, float x, float y, float width,
                                      float height, float minDepth, float maxDepth) {
	(void)encoder;
	(void)x;
	(void)y;
	(void)width;
	(void)height;
	(void)minDepth;
	(void)maxDepth;
}
void wgpuRenderPassEncoderSetScissorRect(WGPURenderPassEncoder encoder, uint32_t x, uint32_t y,
                                         uint32_t width, uint32_t height) {
	(void)encoder;
	(void)x;
	(void)y;
	(void)width;
	(void)height;
}
void wgpuRenderPassEncoderSetBlendConstant(WGPURenderPassEncoder encoder, const WGPUColor *color) {
	(void)encoder;
	(void)color;
}
void wgpuRenderPassEncoderSetStencilReference(WGPURenderPassEncoder encoder, uint32_t reference) {
	(void)encoder;
	(void)reference;
}
WGPUComputePassEncoder wgpuCommandEncoderBeginComputePass(WGPUCommandEncoder encoder,
                                                          const void *descriptor) {
	(void)encoder;
	(void)descriptor;
	return (WGPUComputePassEncoder)(uintptr_t)1;
}
void wgpuComputePassEncoderEnd(WGPUComputePassEncoder encoder) {
	(void)encoder;
}
void wgpuComputePassEncoderRelease(WGPUComputePassEncoder encoder) {
	(void)encoder;
}
void wgpuComputePassEncoderSetPipeline(WGPUComputePassEncoder encoder,
                                       WGPUComputePipeline pipeline) {
	(void)encoder;
	(void)pipeline;
}
void wgpuComputePassEncoderSetBindGroup(WGPUComputePassEncoder encoder, uint32_t groupIndex,
                                        WGPUBindGroup group, size_t dynamicOffsetCount,
                                        const uint32_t *dynamicOffsets) {
	(void)encoder;
	(void)groupIndex;
	(void)group;
	(void)dynamicOffsetCount;
	(void)dynamicOffsets;
}
void wgpuComputePassEncoderDispatchWorkgroups(WGPUComputePassEncoder encoder, uint32_t x,
                                              uint32_t y, uint32_t z) {
	(void)encoder;
	(void)x;
	(void)y;
	(void)z;
}
void wgpuComputePassEncoderDispatchWorkgroupsIndirect(WGPUComputePassEncoder encoder,
                                                      WGPUBuffer buffer, uint64_t offset) {
	(void)encoder;
	(void)buffer;
	(void)offset;
}
