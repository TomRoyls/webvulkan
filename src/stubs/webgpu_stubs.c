/* WebGPU stub implementations for native builds */
/* These are replaced by Emscripten's WebGPU when building for WASM */

#include <webgpu/webgpu.h>
#include <stdlib.h>
#include <string.h>

/* Opaque handle counter - we just return unique pointers */
static uint64_t handle_counter = 0x1000;

static void* make_handle(void) {
    return (void*)(uintptr_t)(++handle_counter);
}

/* Instance */
WGPUInstance wgpuCreateInstance(WGPUInstanceDescriptor const * desc) {
    (void)desc;
    return (WGPUInstance)make_handle();
}

void wgpuInstanceAddRef(WGPUInstance instance) { (void)instance; }
void wgpuInstanceRelease(WGPUInstance instance) { (void)instance; }

/* Adapter request - calls callback synchronously */
WGPUFuture wgpuInstanceRequestAdapter(WGPUInstance instance, 
    WGPU_NULLABLE WGPURequestAdapterOptions const * options,
    WGPURequestAdapterCallbackInfo callbackInfo) {
    (void)instance; (void)options;
    if (callbackInfo.callback) {
        WGPUStringView msg = { .data = NULL, .length = WGPU_STRLEN };
        callbackInfo.callback(WGPURequestAdapterStatus_Success, 
            (WGPUAdapter)make_handle(), msg, callbackInfo.userdata1, callbackInfo.userdata2);
    }
    return (WGPUFuture){0};
}

void wgpuAdapterAddRef(WGPUAdapter adapter) { (void)adapter; }
void wgpuAdapterRelease(WGPUAdapter adapter) { (void)adapter; }

/* Device request - calls callback synchronously */
WGPUFuture wgpuAdapterRequestDevice(WGPUAdapter adapter, 
    WGPU_NULLABLE WGPUDeviceDescriptor const * descriptor,
    WGPURequestDeviceCallbackInfo callbackInfo) {
    (void)adapter; (void)descriptor;
    if (callbackInfo.callback) {
        WGPUStringView msg = { .data = NULL, .length = WGPU_STRLEN };
        callbackInfo.callback(WGPURequestDeviceStatus_Success, 
            (WGPUDevice)make_handle(), msg, callbackInfo.userdata1, callbackInfo.userdata2);
    }
    return (WGPUFuture){0};
}

/* Device */
WGPUQueue wgpuDeviceGetQueue(WGPUDevice device) { (void)device; return (WGPUQueue)make_handle(); }
void wgpuDeviceAddRef(WGPUDevice device) { (void)device; }
void wgpuDeviceRelease(WGPUDevice device) { (void)device; }
void wgpuDeviceDestroy(WGPUDevice device) { (void)device; }

/* Queue */
void wgpuQueueSubmit(WGPUQueue queue, size_t commandCount, WGPUCommandBuffer const * commands) {
    (void)queue; (void)commandCount; (void)commands;
}
void wgpuQueueWriteBuffer(WGPUQueue queue, WGPUBuffer buffer, uint64_t offset, void const * data, size_t size) {
    (void)queue; (void)buffer; (void)offset; (void)data; (void)size;
}
void wgpuQueueAddRef(WGPUQueue queue) { (void)queue; }
void wgpuQueueRelease(WGPUQueue queue) { (void)queue; }

/* Buffer */
WGPUBuffer wgpuDeviceCreateBuffer(WGPUDevice device, WGPUBufferDescriptor const * descriptor) {
    (void)device; (void)descriptor;
    return (WGPUBuffer)make_handle();
}
void wgpuBufferAddRef(WGPUBuffer buffer) { (void)buffer; }
void wgpuBufferRelease(WGPUBuffer buffer) { (void)buffer; }
void * wgpuBufferGetMappedRange(WGPUBuffer buffer, size_t offset, size_t size) {
    (void)buffer; (void)offset; (void)size;
    return NULL;
}
void wgpuBufferUnmap(WGPUBuffer buffer) { (void)buffer; }

/* Texture */
WGPUTexture wgpuDeviceCreateTexture(WGPUDevice device, WGPUTextureDescriptor const * descriptor) {
    (void)device; (void)descriptor;
    return (WGPUTexture)make_handle();
}
WGPUTextureView wgpuTextureCreateView(WGPUTexture texture, WGPU_NULLABLE WGPUTextureViewDescriptor const * descriptor) {
    (void)texture; (void)descriptor;
    return (WGPUTextureView)make_handle();
}
void wgpuTextureAddRef(WGPUTexture texture) { (void)texture; }
void wgpuTextureRelease(WGPUTexture texture) { (void)texture; }
void wgpuTextureViewAddRef(WGPUTextureView textureView) { (void)textureView; }
void wgpuTextureViewRelease(WGPUTextureView textureView) { (void)textureView; }

/* Shader module */
WGPUShaderModule wgpuDeviceCreateShaderModule(WGPUDevice device, WGPUShaderModuleDescriptor const * descriptor) {
    (void)device; (void)descriptor;
    return (WGPUShaderModule)make_handle();
}
void wgpuShaderModuleAddRef(WGPUShaderModule shaderModule) { (void)shaderModule; }
void wgpuShaderModuleRelease(WGPUShaderModule shaderModule) { (void)shaderModule; }

/* Pipeline layout */
WGPUPipelineLayout wgpuDeviceCreatePipelineLayout(WGPUDevice device, WGPUPipelineLayoutDescriptor const * descriptor) {
    (void)device; (void)descriptor;
    return (WGPUPipelineLayout)make_handle();
}
void wgpuPipelineLayoutAddRef(WGPUPipelineLayout pipelineLayout) { (void)pipelineLayout; }
void wgpuPipelineLayoutRelease(WGPUPipelineLayout pipelineLayout) { (void)pipelineLayout; }

/* Bind group layout */
WGPUBindGroupLayout wgpuDeviceCreateBindGroupLayout(WGPUDevice device, WGPUBindGroupLayoutDescriptor const * descriptor) {
    (void)device; (void)descriptor;
    return (WGPUBindGroupLayout)make_handle();
}
void wgpuBindGroupLayoutAddRef(WGPUBindGroupLayout bindGroupLayout) { (void)bindGroupLayout; }
void wgpuBindGroupLayoutRelease(WGPUBindGroupLayout bindGroupLayout) { (void)bindGroupLayout; }

/* Bind group */
WGPUBindGroup wgpuDeviceCreateBindGroup(WGPUDevice device, WGPUBindGroupDescriptor const * descriptor) {
    (void)device; (void)descriptor;
    return (WGPUBindGroup)make_handle();
}
void wgpuBindGroupAddRef(WGPUBindGroup bindGroup) { (void)bindGroup; }
void wgpuBindGroupRelease(WGPUBindGroup bindGroup) { (void)bindGroup; }

/* Sampler */
WGPUSampler wgpuDeviceCreateSampler(WGPUDevice device, WGPU_NULLABLE WGPUSamplerDescriptor const * descriptor) {
    (void)device; (void)descriptor;
    return (WGPUSampler)make_handle();
}
void wgpuSamplerAddRef(WGPUSampler sampler) { (void)sampler; }
void wgpuSamplerRelease(WGPUSampler sampler) { (void)sampler; }

/* Command encoder */
WGPUCommandEncoder wgpuDeviceCreateCommandEncoder(WGPUDevice device, WGPU_NULLABLE WGPUCommandEncoderDescriptor const * descriptor) {
    (void)device; (void)descriptor;
    return (WGPUCommandEncoder)make_handle();
}
WGPUCommandBuffer wgpuCommandEncoderFinish(WGPUCommandEncoder encoder, WGPU_NULLABLE WGPUCommandBufferDescriptor const * descriptor) {
    (void)encoder; (void)descriptor;
    return (WGPUCommandBuffer)make_handle();
}
void wgpuCommandEncoderAddRef(WGPUCommandEncoder commandEncoder) { (void)commandEncoder; }
void wgpuCommandEncoderRelease(WGPUCommandEncoder commandEncoder) { (void)commandEncoder; }
void wgpuCommandBufferAddRef(WGPUCommandBuffer commandBuffer) { (void)commandBuffer; }
void wgpuCommandBufferRelease(WGPUCommandBuffer commandBuffer) { (void)commandBuffer; }

/* Copy operations */
void wgpuCommandEncoderCopyBufferToBuffer(WGPUCommandEncoder commandEncoder, 
    WGPUBuffer source, uint64_t sourceOffset,
    WGPUBuffer destination, uint64_t destinationOffset, uint64_t size) {
    (void)commandEncoder; (void)source; (void)sourceOffset;
    (void)destination; (void)destinationOffset; (void)size;
}
void wgpuCommandEncoderCopyBufferToTexture(WGPUCommandEncoder commandEncoder, 
    WGPUTexelCopyBufferInfo const * source, WGPUTexelCopyTextureInfo const * destination, WGPUExtent3D const * copySize) {
    (void)commandEncoder; (void)source; (void)destination; (void)copySize;
}
void wgpuCommandEncoderCopyTextureToBuffer(WGPUCommandEncoder commandEncoder, 
    WGPUTexelCopyTextureInfo const * source, WGPUTexelCopyBufferInfo const * destination, WGPUExtent3D const * copySize) {
    (void)commandEncoder; (void)source; (void)destination; (void)copySize;
}

/* Render pipeline */
WGPURenderPipeline wgpuDeviceCreateRenderPipeline(WGPUDevice device, WGPURenderPipelineDescriptor const * descriptor) {
    (void)device; (void)descriptor;
    return (WGPURenderPipeline)make_handle();
}
void wgpuRenderPipelineAddRef(WGPURenderPipeline renderPipeline) { (void)renderPipeline; }
void wgpuRenderPipelineRelease(WGPURenderPipeline renderPipeline) { (void)renderPipeline; }

/* Compute pipeline */
WGPUComputePipeline wgpuDeviceCreateComputePipeline(WGPUDevice device, WGPUComputePipelineDescriptor const * descriptor) {
    (void)device; (void)descriptor;
    return (WGPUComputePipeline)make_handle();
}
void wgpuComputePipelineAddRef(WGPUComputePipeline computePipeline) { (void)computePipeline; }
void wgpuComputePipelineRelease(WGPUComputePipeline computePipeline) { (void)computePipeline; }

/* Render pass encoder */
WGPURenderPassEncoder wgpuCommandEncoderBeginRenderPass(WGPUCommandEncoder encoder, WGPURenderPassDescriptor const * descriptor) {
    (void)encoder; (void)descriptor;
    return (WGPURenderPassEncoder)make_handle();
}
void wgpuRenderPassEncoderSetPipeline(WGPURenderPassEncoder renderPassEncoder, WGPURenderPipeline pipeline) {
    (void)renderPassEncoder; (void)pipeline;
}
void wgpuRenderPassEncoderSetBindGroup(WGPURenderPassEncoder renderPassEncoder, uint32_t groupIndex, WGPU_NULLABLE WGPUBindGroup bindGroup, size_t dynamicOffsetCount, uint32_t const * dynamicOffsets) {
    (void)renderPassEncoder; (void)groupIndex; (void)bindGroup; (void)dynamicOffsetCount; (void)dynamicOffsets;
}
void wgpuRenderPassEncoderSetVertexBuffer(WGPURenderPassEncoder renderPassEncoder, uint32_t slot, WGPU_NULLABLE WGPUBuffer buffer, uint64_t offset, uint64_t size) {
    (void)renderPassEncoder; (void)slot; (void)buffer; (void)offset; (void)size;
}
void wgpuRenderPassEncoderSetIndexBuffer(WGPURenderPassEncoder renderPassEncoder, WGPUBuffer buffer, WGPUIndexFormat format, uint64_t offset, uint64_t size) {
    (void)renderPassEncoder; (void)buffer; (void)format; (void)offset; (void)size;
}
void wgpuRenderPassEncoderSetBlendConstant(WGPURenderPassEncoder renderPassEncoder, WGPUColor const * color) {
    (void)renderPassEncoder; (void)color;
}
void wgpuRenderPassEncoderDraw(WGPURenderPassEncoder renderPassEncoder, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
    (void)renderPassEncoder; (void)vertexCount; (void)instanceCount; (void)firstVertex; (void)firstInstance;
}
void wgpuRenderPassEncoderDrawIndexed(WGPURenderPassEncoder renderPassEncoder, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t baseVertex, uint32_t firstInstance) {
    (void)renderPassEncoder; (void)indexCount; (void)instanceCount; (void)firstIndex; (void)baseVertex; (void)firstInstance;
}
void wgpuRenderPassEncoderDrawIndirect(WGPURenderPassEncoder renderPassEncoder, WGPUBuffer indirectBuffer, uint64_t indirectOffset) {
    (void)renderPassEncoder; (void)indirectBuffer; (void)indirectOffset;
}
void wgpuRenderPassEncoderDrawIndexedIndirect(WGPURenderPassEncoder renderPassEncoder, WGPUBuffer indirectBuffer, uint64_t indirectOffset) {
    (void)renderPassEncoder; (void)indirectBuffer; (void)indirectOffset;
}
void wgpuRenderPassEncoderEnd(WGPURenderPassEncoder renderPassEncoder) { (void)renderPassEncoder; }
void wgpuRenderPassEncoderAddRef(WGPURenderPassEncoder renderPassEncoder) { (void)renderPassEncoder; }
void wgpuRenderPassEncoderRelease(WGPURenderPassEncoder renderPassEncoder) { (void)renderPassEncoder; }

/* Compute pass encoder */
WGPUComputePassEncoder wgpuCommandEncoderBeginComputePass(WGPUCommandEncoder encoder, WGPU_NULLABLE WGPUComputePassDescriptor const * descriptor) {
    (void)encoder; (void)descriptor;
    return (WGPUComputePassEncoder)make_handle();
}
void wgpuComputePassEncoderSetPipeline(WGPUComputePassEncoder computePassEncoder, WGPUComputePipeline pipeline) {
    (void)computePassEncoder; (void)pipeline;
}
void wgpuComputePassEncoderSetBindGroup(WGPUComputePassEncoder computePassEncoder, uint32_t groupIndex, WGPU_NULLABLE WGPUBindGroup bindGroup, size_t dynamicOffsetCount, uint32_t const * dynamicOffsets) {
    (void)computePassEncoder; (void)groupIndex; (void)bindGroup; (void)dynamicOffsetCount; (void)dynamicOffsets;
}
void wgpuComputePassEncoderDispatchWorkgroups(WGPUComputePassEncoder computePassEncoder, uint32_t workgroupCountX, uint32_t workgroupCountY, uint32_t workgroupCountZ) {
    (void)computePassEncoder; (void)workgroupCountX; (void)workgroupCountY; (void)workgroupCountZ;
}
void wgpuComputePassEncoderEnd(WGPUComputePassEncoder computePassEncoder) { (void)computePassEncoder; }
void wgpuComputePassEncoderAddRef(WGPUComputePassEncoder computePassEncoder) { (void)computePassEncoder; }
void wgpuComputePassEncoderRelease(WGPUComputePassEncoder computePassEncoder) { (void)computePassEncoder; }
