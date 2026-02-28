/*
 * Minimal WebGPU C header stubs for native testing.
 * This provides type definitions only - no actual WebGPU functionality.
 * For real builds, use Emscripten or Dawn/wgpu-native headers.
 */

#ifndef WEBGPU_H_
#define WEBGPU_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque handle types */
typedef struct WGPUInstanceImpl* WGPUInstance;
typedef struct WGPUAdapterImpl* WGPUAdapter;
typedef struct WGPUDeviceImpl* WGPUDevice;
typedef struct WGPUQueueImpl* WGPUQueue;
typedef struct WGPUBufferImpl* WGPUBuffer;
typedef struct WGPUTextureImpl* WGPUTexture;
typedef struct WGPUTextureViewImpl* WGPUTextureView;
typedef struct WGPUShaderModuleImpl* WGPUShaderModule;
typedef struct WGPUPipelineLayoutImpl* WGPUPipelineLayout;
typedef struct WGPURenderPipelineImpl* WGPURenderPipeline;
typedef struct WGPUComputePipelineImpl* WGPUComputePipeline;
typedef struct WGPUCommandEncoderImpl* WGPUCommandEncoder;
typedef struct WGPURenderPassEncoderImpl* WGPURenderPassEncoder;
typedef struct WGPUComputePassEncoderImpl* WGPUComputePassEncoder;
typedef struct WGPUBindGroupLayoutImpl* WGPUBindGroupLayout;
typedef struct WGPUBindGroupImpl* WGPUBindGroup;
typedef struct WGPUSamplerImpl* WGPUSampler;
typedef struct WGPUCommandBufferImpl* WGPUCommandBuffer;

/* Adapter info struct */
typedef struct WGPUAdapterInfo {
    const char* vendor;
    const char* architecture;
    const char* device;
    const char* description;
} WGPUAdapterInfo;

/* Bind group entry */
typedef struct WGPUBindGroupEntry {
    uint32_t binding;
    WGPUBuffer buffer;
    uint64_t offset;
    uint64_t size;
    WGPUSampler sampler;
    WGPUTextureView textureView;
} WGPUBindGroupEntry;

/* Common enums */
typedef enum WGPUTextureFormat {
    WGPUTextureFormat_Undefined = 0,
    WGPUTextureFormat_R8Unorm = 1,
    WGPUTextureFormat_R8Snorm = 2,
    WGPUTextureFormat_R8Uint = 3,
    WGPUTextureFormat_R8Sint = 4,
    WGPUTextureFormat_R16Uint = 5,
    WGPUTextureFormat_R16Sint = 6,
    WGPUTextureFormat_R16Float = 7,
    WGPUTextureFormat_RG8Unorm = 8,
    WGPUTextureFormat_RG8Snorm = 9,
    WGPUTextureFormat_RG8Uint = 10,
    WGPUTextureFormat_RG8Sint = 11,
    WGPUTextureFormat_R32Float = 12,
    WGPUTextureFormat_R32Uint = 13,
    WGPUTextureFormat_R32Sint = 14,
    WGPUTextureFormat_RG16Uint = 15,
    WGPUTextureFormat_RG16Sint = 16,
    WGPUTextureFormat_RG16Float = 17,
    WGPUTextureFormat_RGBA8Unorm = 18,
    WGPUTextureFormat_RGBA8UnormSrgb = 19,
    WGPUTextureFormat_RGBA8Snorm = 20,
    WGPUTextureFormat_RGBA8Uint = 21,
    WGPUTextureFormat_RGBA8Sint = 22,
    WGPUTextureFormat_BGRA8Unorm = 23,
    WGPUTextureFormat_BGRA8UnormSrgb = 24,
    WGPUTextureFormat_RGB10A2Uint = 25,
    WGPUTextureFormat_RGB10A2Unorm = 26,
    WGPUTextureFormat_RG11B10Ufloat = 27,
    WGPUTextureFormat_RGB9E5Ufloat = 28,
    WGPUTextureFormat_RG32Float = 29,
    WGPUTextureFormat_RG32Uint = 30,
    WGPUTextureFormat_RG32Sint = 31,
    WGPUTextureFormat_RGBA16Float = 32,
    WGPUTextureFormat_RGBA16Uint = 33,
    WGPUTextureFormat_RGBA16Sint = 34,
    WGPUTextureFormat_RGBA32Float = 35,
    WGPUTextureFormat_RGBA32Uint = 36,
    WGPUTextureFormat_RGBA32Sint = 37,
    WGPUTextureFormat_Depth16Unorm = 38,
    WGPUTextureFormat_Depth24Plus = 39,
    WGPUTextureFormat_Depth24PlusStencil8 = 40,
    WGPUTextureFormat_Depth32Float = 41,
    WGPUTextureFormat_Stencil8 = 42,
} WGPUTextureFormat;

#ifdef __cplusplus
}
#endif

#endif /* WEBGPU_H_ */
