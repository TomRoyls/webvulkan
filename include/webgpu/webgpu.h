#ifndef WEBGPU_H_STUB
#define WEBGPU_H_STUB

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct WGPUInstanceImpl* WGPUInstance;
typedef struct WGPUAdapterImpl* WGPUAdapter;
typedef struct WGPUDeviceImpl* WGPUDevice;
typedef struct WGPUQueueImpl* WGPUQueue;
typedef struct WGPUBufferImpl* WGPUBuffer;
typedef struct WGPUTextureImpl* WGPUTexture;
typedef struct WGPUTextureViewImpl* WGPUTextureView;
typedef struct WGPUShaderModuleImpl* WGPUShaderModule;
typedef struct WGPURenderPipelineImpl* WGPURenderPipeline;
typedef struct WGPUComputePipelineImpl* WGPUComputePipeline;
typedef struct WGPUPipelineLayoutImpl* WGPUPipelineLayout;
typedef struct WGPUBindGroupLayoutImpl* WGPUBindGroupLayout;
typedef struct WGPUBindGroupImpl* WGPUBindGroup;
typedef struct WGPUSamplerImpl* WGPUSampler;
typedef struct WGPUCommandEncoderImpl* WGPUCommandEncoder;
typedef struct WGPURenderPassEncoderImpl* WGPURenderPassEncoder;
typedef struct WGPUComputePassEncoderImpl* WGPUComputePassEncoder;
typedef struct WGPUCommandBufferImpl* WGPUCommandBuffer;

typedef struct WGPUAdapterInfo {
    const char* vendor;
    const char* architecture;
    const char* device;
    const char* description;
} WGPUAdapterInfo;

typedef enum WGPUTextureFormat {
    WGPUTextureFormat_Undefined = 0,
    WGPUTextureFormat_RGBA8Unorm = 1,
    WGPUTextureFormat_BGRA8Unorm = 2,
    WGPUTextureFormat_Depth24Plus = 3,
    WGPUTextureFormat_Depth24PlusStencil8 = 4,
    WGPUTextureFormat_Depth32Float = 5,
} WGPUTextureFormat;

typedef enum WGPUTextureViewDimension {
    WGPUTextureViewDimension_1D,
    WGPUTextureViewDimension_2D,
    WGPUTextureViewDimension_2DArray,
    WGPUTextureViewDimension_Cube,
    WGPUTextureViewDimension_CubeArray,
    WGPUTextureViewDimension_3D,
} WGPUTextureViewDimension;

typedef enum WGPUBufferUsage {
    WGPUBufferUsage_None = 0,
    WGPUBufferUsage_MapRead = 1,
    WGPUBufferUsage_MapWrite = 2,
    WGPUBufferUsage_CopySrc = 4,
    WGPUBufferUsage_CopyDst = 8,
    WGPUBufferUsage_Index = 16,
    WGPUBufferUsage_Vertex = 32,
    WGPUBufferUsage_Uniform = 64,
    WGPUBufferUsage_Storage = 128,
    WGPUBufferUsage_Indirect = 256,
    WGPUBufferUsage_QueryResolve = 512,
} WGPUBufferUsage;

typedef enum WGPUTextureUsage {
    WGPUTextureUsage_None = 0,
    WGPUTextureUsage_CopySrc = 1,
    WGPUTextureUsage_CopyDst = 2,
    WGPUTextureUsage_TextureBinding = 4,
    WGPUTextureUsage_StorageBinding = 8,
    WGPUTextureUsage_RenderAttachment = 16,
} WGPUTextureUsage;

typedef enum WGPUDeviceLostReason {
    WGPUDeviceLostReason_Unknown = 0,
    WGPUDeviceLostReason_Destroyed = 1,
} WGPUDeviceLostReason;

typedef enum WGPUErrorType {
    WGPUErrorType_NoError = 0,
    WGPUErrorType_Validation = 1,
    WGPUErrorType_OutOfMemory = 2,
    WGPUErrorType_Internal = 3,
} WGPUErrorType;

typedef void (*WGPUDeviceLostCallback)(WGPUDeviceLostReason reason, const char* message, void* userdata);
typedef void (*WGPUErrorCallback)(WGPUErrorType type, const char* message, void* userdata);

typedef enum WGPUShaderStage {
    WGPUShaderStage_None = 0,
    WGPUShaderStage_Vertex = 1,
    WGPUShaderStage_Fragment = 2,
    WGPUShaderStage_Compute = 4,
} WGPUShaderStage;

typedef enum WGPUSamplerBindingType {
    WGPUSamplerBindingType_Filtering,
    WGPUSamplerBindingType_NonFiltering,
    WGPUSamplerBindingType_Comparison,
} WGPUSamplerBindingType;

typedef enum WGPUTextureSampleType {
    WGPUTextureSampleType_Float,
    WGPUTextureSampleType_UnfilterableFloat,
    WGPUTextureSampleType_Depth,
    WGPUTextureSampleType_Sint,
    WGPUTextureSampleType_Uint,
} WGPUTextureSampleType;

typedef enum WGPUBufferBindingType {
    WGPUBufferBindingType_Uniform,
    WGPUBufferBindingType_Storage,
    WGPUBufferBindingType_ReadOnlyStorage,
} WGPUBufferBindingType;

typedef enum WGPUStorageTextureAccess {
    WGPUStorageTextureAccess_WriteOnly,
    WGPUStorageTextureAccess_ReadOnly,
    WGPUStorageTextureAccess_ReadWrite,
} WGPUStorageTextureAccess;

typedef enum WGPUSType {
    WGPUSType_ShaderSourceWGSL = 1,
} WGPUSType;

typedef struct WGPUChainedStruct {
    const struct WGPUChainedStruct* next;
    WGPUSType sType;
} WGPUChainedStruct;

typedef struct WGPUShaderModuleWGSLDescriptor {
    WGPUChainedStruct chain;
    const char* code;
} WGPUShaderModuleWGSLDescriptor;

typedef struct WGPUInstanceDescriptor {
    const WGPUChainedStruct* nextInChain;
} WGPUInstanceDescriptor;

typedef struct WGPUDeviceDescriptor {
    const WGPUChainedStruct* nextInChain;
    WGPUDeviceLostCallback deviceLostCallback;
    void* deviceLostUserdata;
    WGPUErrorCallback uncapturedErrorCallback;
    void* uncapturedErrorUserdata;
} WGPUDeviceDescriptor;

typedef struct WGPUBufferDescriptor {
    const WGPUChainedStruct* nextInChain;
    const char* label;
    WGPUBufferUsage usage;
    uint64_t size;
    bool mappedAtCreation;
} WGPUBufferDescriptor;

typedef struct WGPUTextureDescriptor {
    const WGPUChainedStruct* nextInChain;
    const char* label;
    WGPUTextureUsage usage;
    uint32_t dimension;
    uint64_t size[3];
    WGPUTextureFormat format;
    uint32_t mipLevelCount;
    uint32_t sampleCount;
} WGPUTextureDescriptor;

typedef struct WGPUTextureViewDescriptor {
    const WGPUChainedStruct* nextInChain;
    const char* label;
    WGPUTextureFormat format;
    uint32_t dimension;
    uint32_t baseMipLevel;
    uint32_t mipLevelCount;
    uint32_t baseArrayLayer;
    uint32_t arrayLayerCount;
    uint32_t aspect;
} WGPUTextureViewDescriptor;

typedef struct WGPUShaderModuleDescriptor {
    const WGPUChainedStruct* nextInChain;
    const char* label;
} WGPUShaderModuleDescriptor;

typedef struct WGPUPipelineLayoutDescriptor {
    const WGPUChainedStruct* nextInChain;
    const char* label;
    uint32_t bindGroupLayoutCount;
    const WGPUBindGroupLayout* bindGroupLayouts;
} WGPUPipelineLayoutDescriptor;

typedef struct WGPUBindGroupLayoutEntry {
    uint32_t binding;
    WGPUShaderStage visibility;
    struct {
        WGPUBufferBindingType type;
        bool hasDynamicOffset;
        uint64_t minBindingSize;
    } buffer;
    struct {
        WGPUSamplerBindingType type;
    } sampler;
    struct {
        WGPUTextureSampleType sampleType;
        WGPUTextureViewDimension viewDimension;
        bool multisampled;
    } texture;
    struct {
        WGPUStorageTextureAccess access;
        WGPUTextureFormat format;
        WGPUTextureViewDimension viewDimension;
    } storageTexture;
} WGPUBindGroupLayoutEntry;

typedef struct WGPUBindGroupLayoutDescriptor {
    const WGPUChainedStruct* nextInChain;
    const char* label;
    uint32_t entryCount;
    const WGPUBindGroupLayoutEntry* entries;
} WGPUBindGroupLayoutDescriptor;

typedef struct WGPURenderPipelineDescriptor {
    const WGPUChainedStruct* nextInChain;
    const char* label;
} WGPURenderPipelineDescriptor;

typedef struct WGPUComputePipelineDescriptor {
    const WGPUChainedStruct* nextInChain;
    const char* label;
    WGPUShaderModule module;
    const char* entryPoint;
    WGPUPipelineLayout layout;
} WGPUComputePipelineDescriptor;

typedef struct WGPUSamplerDescriptor {
    const WGPUChainedStruct* nextInChain;
    const char* label;
    uint32_t addressModeU;
    uint32_t addressModeV;
    uint32_t addressModeW;
    uint32_t magFilter;
    uint32_t minFilter;
    uint32_t mipmapFilter;
    float lodMinClamp;
    float lodMaxClamp;
    uint32_t compare;
    uint16_t maxAnisotropy;
} WGPUSamplerDescriptor;

typedef struct WGPUCommandBufferDescriptor {
    const WGPUChainedStruct* nextInChain;
    const char* label;
} WGPUCommandBufferDescriptor;

static inline WGPUInstance wgpuCreateInstance(const WGPUInstanceDescriptor* desc) {
    (void)desc;
    return (WGPUInstance)1;
}

static inline void wgpuInstanceRelease(WGPUInstance instance) {
    (void)instance;
}

static inline WGPUAdapter wgpuInstanceRequestAdapter(WGPUInstance instance, const void* opts) {
    (void)instance; (void)opts;
    return (WGPUAdapter)1;
}

static inline WGPUDevice wgpuAdapterRequestDevice(WGPUAdapter adapter, const WGPUDeviceDescriptor* desc) {
    (void)adapter; (void)desc;
    return (WGPUDevice)1;
}

static inline WGPUDevice wgpuAdapterCreateDevice(WGPUAdapter adapter, const WGPUDeviceDescriptor* desc) {
    (void)adapter; (void)desc;
    return (WGPUDevice)1;
}

static inline WGPUDevice wgpuCreateDevice(const WGPUDeviceDescriptor* desc) {
    (void)desc;
    return (WGPUDevice)1;
}

static inline WGPUQueue wgpuDeviceGetQueue(WGPUDevice device) {
    (void)device;
    return (WGPUQueue)1;
}

static inline WGPUBuffer wgpuDeviceCreateBuffer(WGPUDevice device, const WGPUBufferDescriptor* desc) {
    (void)device; (void)desc;
    return (WGPUBuffer)1;
}

static inline WGPUTexture wgpuDeviceCreateTexture(WGPUDevice device, const WGPUTextureDescriptor* desc) {
    (void)device; (void)desc;
    return (WGPUTexture)1;
}

static inline WGPUShaderModule wgpuDeviceCreateShaderModule(WGPUDevice device, const WGPUShaderModuleDescriptor* desc) {
    (void)device; (void)desc;
    return (WGPUShaderModule)1;
}

static inline WGPUPipelineLayout wgpuDeviceCreatePipelineLayout(WGPUDevice device, const WGPUPipelineLayoutDescriptor* desc) {
    (void)device; (void)desc;
    return (WGPUPipelineLayout)1;
}

static inline WGPUBindGroupLayout wgpuDeviceCreateBindGroupLayout(WGPUDevice device, const WGPUBindGroupLayoutDescriptor* desc) {
    (void)device; (void)desc;
    return (WGPUBindGroupLayout)1;
}

static inline WGPUSampler wgpuDeviceCreateSampler(WGPUDevice device, const WGPUSamplerDescriptor* desc) {
    (void)device; (void)desc;
    return (WGPUSampler)1;
}

static inline WGPUCommandEncoder wgpuDeviceCreateCommandEncoder(WGPUDevice device, const void* desc) {
    (void)device; (void)desc;
    return (WGPUCommandEncoder)1;
}

static inline WGPUCommandBuffer wgpuCommandEncoderFinish(WGPUCommandEncoder encoder, const WGPUCommandBufferDescriptor* desc) {
    (void)encoder; (void)desc;
    return (WGPUCommandBuffer)1;
}

static inline void wgpuQueueSubmit(WGPUQueue queue, uint32_t count, WGPUCommandBuffer* buffers) {
    (void)queue; (void)count; (void)buffers;
}

static inline WGPUTextureView wgpuTextureCreateView(WGPUTexture texture, const WGPUTextureViewDescriptor* desc) {
    (void)texture; (void)desc;
    return (WGPUTextureView)1;
}

static inline WGPUTextureFormat wgpuTextureGetFormat(WGPUTexture texture) {
    (void)texture;
    return WGPUTextureFormat_RGBA8Unorm;
}

static inline void wgpuQueueWriteBuffer(WGPUQueue queue, WGPUBuffer buffer, uint64_t offset, const void* data, size_t size) {
    (void)queue; (void)buffer; (void)offset; (void)data; (void)size;
}

static inline void wgpuBufferRelease(WGPUBuffer buffer) { (void)buffer; }
static inline void wgpuTextureRelease(WGPUTexture texture) { (void)texture; }
static inline void wgpuTextureViewRelease(WGPUTextureView view) { (void)view; }
static inline void wgpuShaderModuleRelease(WGPUShaderModule module) { (void)module; }
static inline void wgpuPipelineLayoutRelease(WGPUPipelineLayout layout) { (void)layout; }
static inline void wgpuBindGroupLayoutRelease(WGPUBindGroupLayout layout) { (void)layout; }
static inline void wgpuSamplerRelease(WGPUSampler sampler) { (void)sampler; }
static inline void wgpuDeviceRelease(WGPUDevice device) { (void)device; }
static inline void wgpuAdapterRelease(WGPUAdapter adapter) { (void)adapter; }
static inline void wgpuQueueRelease(WGPUQueue queue) { (void)queue; }
static inline void wgpuQueueReference(WGPUQueue queue) { (void)queue; }
static inline void wgpuCommandBufferRelease(WGPUCommandBuffer cmd) { (void)cmd; }
static inline void wgpuCommandEncoderRelease(WGPUCommandEncoder encoder) { (void)encoder; }

#ifdef __cplusplus
}
#endif

#endif
