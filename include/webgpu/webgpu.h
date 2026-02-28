/*
 * Minimal WebGPU C header stubs for native testing.
 * Provides type definitions only — no actual WebGPU functionality.
 * For real builds, use Emscripten or Dawn/wgpu-native headers.
 */

#ifndef WEBGPU_H_
#define WEBGPU_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Opaque handle types ---- */
typedef struct WGPUInstanceImpl *WGPUInstance;
typedef struct WGPUAdapterImpl *WGPUAdapter;
typedef struct WGPUDeviceImpl *WGPUDevice;
typedef struct WGPUQueueImpl *WGPUQueue;
typedef struct WGPUBufferImpl *WGPUBuffer;
typedef struct WGPUTextureImpl *WGPUTexture;
typedef struct WGPUTextureViewImpl *WGPUTextureView;
typedef struct WGPUShaderModuleImpl *WGPUShaderModule;
typedef struct WGPUPipelineLayoutImpl *WGPUPipelineLayout;
typedef struct WGPURenderPipelineImpl *WGPURenderPipeline;
typedef struct WGPUComputePipelineImpl *WGPUComputePipeline;
typedef struct WGPUCommandEncoderImpl *WGPUCommandEncoder;
typedef struct WGPURenderPassEncoderImpl *WGPURenderPassEncoder;
typedef struct WGPUComputePassEncoderImpl *WGPUComputePassEncoder;
typedef struct WGPUBindGroupLayoutImpl *WGPUBindGroupLayout;
typedef struct WGPUBindGroupImpl *WGPUBindGroup;
typedef struct WGPUSamplerImpl *WGPUSampler;
typedef struct WGPUCommandBufferImpl *WGPUCommandBuffer;

/* ---- String view ---- */
typedef struct WGPUStringView {
    const char *data;
    size_t length;
} WGPUStringView;
#define WGPU_STRLEN ((size_t)-1)
#define WGPU_STRING_VIEW_INIT \
    {                         \
        .data = NULL,         \
        .length = 0,          \
    }

/* ---- Chained struct ---- */
typedef struct WGPUChainedStruct {
    const struct WGPUChainedStruct *next;
    uint32_t sType;
} WGPUChainedStruct;

/* ---- sType constants ---- */
typedef enum WGPUSType {
    WGPUSType_ShaderSourceWGSL = 0x00050001,
} WGPUSType;

/* ---- Enumerations ---- */
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
    WGPUTextureFormat_Depth32FloatStencil8 = 43,
    WGPUTextureFormat_BC1RGBAUnorm = 44,
    WGPUTextureFormat_BC1RGBAUnormSrgb = 45,
    WGPUTextureFormat_BC2RGBAUnorm = 46,
    WGPUTextureFormat_BC2RGBAUnormSrgb = 47,
    WGPUTextureFormat_BC3RGBAUnorm = 48,
    WGPUTextureFormat_BC3RGBAUnormSrgb = 49,
    WGPUTextureFormat_BC4RUnorm = 50,
    WGPUTextureFormat_BC4RSnorm = 51,
    WGPUTextureFormat_BC5RGUnorm = 52,
    WGPUTextureFormat_BC5RGSnorm = 53,
    WGPUTextureFormat_BC7RGBAUnorm = 54,
    WGPUTextureFormat_BC7RGBAUnormSrgb = 55,
} WGPUTextureFormat;

typedef enum WGPUTextureDimension {
    WGPUTextureDimension_1D = 0,
    WGPUTextureDimension_2D = 1,
    WGPUTextureDimension_3D = 2,
} WGPUTextureDimension;

typedef enum WGPUTextureViewDimension {
    WGPUTextureViewDimension_Undefined = 0,
    WGPUTextureViewDimension_1D = 1,
    WGPUTextureViewDimension_2D = 2,
    WGPUTextureViewDimension_2DArray = 3,
    WGPUTextureViewDimension_Cube = 4,
    WGPUTextureViewDimension_CubeArray = 5,
    WGPUTextureViewDimension_3D = 6,
} WGPUTextureViewDimension;

typedef enum WGPUTextureAspect {
    WGPUTextureAspect_All = 0,
    WGPUTextureAspect_StencilOnly = 1,
    WGPUTextureAspect_DepthOnly = 2,
} WGPUTextureAspect;

typedef uint32_t WGPUTextureUsage;
#define WGPUTextureUsage_None ((WGPUTextureUsage)0)
#define WGPUTextureUsage_CopySrc ((WGPUTextureUsage)1)
#define WGPUTextureUsage_CopyDst ((WGPUTextureUsage)2)
#define WGPUTextureUsage_TextureBinding ((WGPUTextureUsage)4)
#define WGPUTextureUsage_StorageBinding ((WGPUTextureUsage)8)
#define WGPUTextureUsage_RenderAttachment ((WGPUTextureUsage)16)

typedef uint32_t WGPUBufferUsage;
#define WGPUBufferUsage_None ((WGPUBufferUsage)0)
#define WGPUBufferUsage_MapRead ((WGPUBufferUsage)1)
#define WGPUBufferUsage_MapWrite ((WGPUBufferUsage)2)
#define WGPUBufferUsage_CopySrc ((WGPUBufferUsage)4)
#define WGPUBufferUsage_CopyDst ((WGPUBufferUsage)8)
#define WGPUBufferUsage_Index ((WGPUBufferUsage)16)
#define WGPUBufferUsage_Vertex ((WGPUBufferUsage)32)
#define WGPUBufferUsage_Uniform ((WGPUBufferUsage)64)
#define WGPUBufferUsage_Storage ((WGPUBufferUsage)128)
#define WGPUBufferUsage_Indirect ((WGPUBufferUsage)256)
#define WGPUBufferUsage_QueryResolve ((WGPUBufferUsage)512)

typedef uint32_t WGPUShaderStage;
#define WGPUShaderStage_None ((WGPUShaderStage)0)
#define WGPUShaderStage_Vertex ((WGPUShaderStage)1)
#define WGPUShaderStage_Fragment ((WGPUShaderStage)2)
#define WGPUShaderStage_Compute ((WGPUShaderStage)4)

#define WGPU_COPY_STRIDE_UNDEFINED (0xFFFFFFFFu)
typedef uint32_t WGPUColorWriteMask;
#define WGPUColorWriteMask_None ((WGPUColorWriteMask)0)
#define WGPUColorWriteMask_Red ((WGPUColorWriteMask)1)
#define WGPUColorWriteMask_Green ((WGPUColorWriteMask)2)
#define WGPUColorWriteMask_Blue ((WGPUColorWriteMask)4)
#define WGPUColorWriteMask_Alpha ((WGPUColorWriteMask)8)
#define WGPUColorWriteMask_All ((WGPUColorWriteMask)0xF)

typedef enum WGPUVertexFormat {
    WGPUVertexFormat_Undefined = 0,
    WGPUVertexFormat_Float32 = 1,
    WGPUVertexFormat_Float32x2 = 2,
    WGPUVertexFormat_Float32x3 = 3,
    WGPUVertexFormat_Float32x4 = 4,
    WGPUVertexFormat_Sint32 = 5,
    WGPUVertexFormat_Sint32x2 = 6,
    WGPUVertexFormat_Sint32x3 = 7,
    WGPUVertexFormat_Sint32x4 = 8,
    WGPUVertexFormat_Uint32 = 9,
    WGPUVertexFormat_Uint32x2 = 10,
    WGPUVertexFormat_Uint32x3 = 11,
    WGPUVertexFormat_Uint32x4 = 12,
} WGPUVertexFormat;

typedef enum WGPUVertexStepMode {
    WGPUVertexStepMode_Vertex = 0,
    WGPUVertexStepMode_Instance = 1,
} WGPUVertexStepMode;

typedef enum WGPUPrimitiveTopology {
    WGPUPrimitiveTopology_PointList = 0,
    WGPUPrimitiveTopology_LineList = 1,
    WGPUPrimitiveTopology_LineStrip = 2,
    WGPUPrimitiveTopology_TriangleList = 3,
    WGPUPrimitiveTopology_TriangleStrip = 4,
} WGPUPrimitiveTopology;

typedef enum WGPUIndexFormat {
    WGPUIndexFormat_Undefined = 0,
    WGPUIndexFormat_Uint16 = 1,
    WGPUIndexFormat_Uint32 = 2,
} WGPUIndexFormat;

typedef enum WGPUFrontFace {
    WGPUFrontFace_CCW = 0,
    WGPUFrontFace_CW = 1,
} WGPUFrontFace;

typedef enum WGPUCullMode {
    WGPUCullMode_None = 0,
    WGPUCullMode_Front = 1,
    WGPUCullMode_Back = 2,
} WGPUCullMode;

typedef enum WGPULoadOp {
    WGPULoadOp_Undefined = 0,
    WGPULoadOp_Clear = 1,
    WGPULoadOp_Load = 2,
} WGPULoadOp;

typedef enum WGPUStoreOp {
    WGPUStoreOp_Undefined = 0,
    WGPUStoreOp_Store = 1,
    WGPUStoreOp_Discard = 2,
} WGPUStoreOp;

typedef enum WGPUAddressMode {
    WGPUAddressMode_ClampToEdge = 0,
    WGPUAddressMode_Repeat = 1,
    WGPUAddressMode_MirrorRepeat = 2,
} WGPUAddressMode;

typedef enum WGPUFilterMode {
    WGPUFilterMode_Nearest = 0,
    WGPUFilterMode_Linear = 1,
} WGPUFilterMode;

typedef enum WGPUMipmapFilterMode {
    WGPUMipmapFilterMode_Nearest = 0,
    WGPUMipmapFilterMode_Linear = 1,
} WGPUMipmapFilterMode;

typedef enum WGPUCompareFunction {
    WGPUCompareFunction_Undefined = 0,
    WGPUCompareFunction_Never = 1,
    WGPUCompareFunction_Less = 2,
    WGPUCompareFunction_LessEqual = 3,
    WGPUCompareFunction_Greater = 4,
    WGPUCompareFunction_GreaterEqual = 5,
    WGPUCompareFunction_Equal = 6,
    WGPUCompareFunction_NotEqual = 7,
    WGPUCompareFunction_Always = 8,
} WGPUCompareFunction;

typedef enum WGPUBufferBindingType {
    WGPUBufferBindingType_BindingNotUsed = 0,
    WGPUBufferBindingType_Uniform = 1,
    WGPUBufferBindingType_Storage = 2,
    WGPUBufferBindingType_ReadOnlyStorage = 3,
} WGPUBufferBindingType;

typedef enum WGPUSamplerBindingType {
    WGPUSamplerBindingType_BindingNotUsed = 0,
    WGPUSamplerBindingType_Filtering = 1,
    WGPUSamplerBindingType_NonFiltering = 2,
    WGPUSamplerBindingType_Comparison = 3,
} WGPUSamplerBindingType;

typedef enum WGPUTextureSampleType {
    WGPUTextureSampleType_BindingNotUsed = 0,
    WGPUTextureSampleType_Float = 1,
    WGPUTextureSampleType_UnfilterableFloat = 2,
    WGPUTextureSampleType_Depth = 3,
    WGPUTextureSampleType_Sint = 4,
    WGPUTextureSampleType_Uint = 5,
} WGPUTextureSampleType;

typedef enum WGPUStorageTextureAccess {
    WGPUStorageTextureAccess_BindingNotUsed = 0,
    WGPUStorageTextureAccess_WriteOnly = 1,
    WGPUStorageTextureAccess_ReadOnly = 2,
    WGPUStorageTextureAccess_ReadWrite = 3,
} WGPUStorageTextureAccess;

/* ---- Simple value structs ---- */
typedef struct WGPUColor {
    double r, g, b, a;
} WGPUColor;

typedef struct WGPUExtent3D {
    uint32_t width;
    uint32_t height;
    uint32_t depthOrArrayLayers;
} WGPUExtent3D;

typedef struct WGPUOrigin3D {
    uint32_t x, y, z;
} WGPUOrigin3D;

/* ---- Descriptor structs ---- */
typedef struct WGPUInstanceDescriptor {
    const WGPUChainedStruct *nextInChain;
} WGPUInstanceDescriptor;

typedef struct WGPUDeviceDescriptor {
    const WGPUChainedStruct *nextInChain;
    WGPUStringView label;
} WGPUDeviceDescriptor;
#define WGPU_DEVICE_DESCRIPTOR_INIT \
    {                               \
        .nextInChain = NULL,        \
        .label = WGPU_STRING_VIEW_INIT, \
    }

typedef struct WGPUBufferDescriptor {
    const WGPUChainedStruct *nextInChain;
    WGPUStringView label;
    WGPUBufferUsage usage;
    uint64_t size;
    int mappedAtCreation;
} WGPUBufferDescriptor;

typedef struct WGPUTextureDescriptor {
    const WGPUChainedStruct *nextInChain;
    WGPUStringView label;
    WGPUTextureUsage usage;
    WGPUTextureDimension dimension;
    WGPUExtent3D size;
    WGPUTextureFormat format;
    uint32_t mipLevelCount;
    uint32_t sampleCount;
} WGPUTextureDescriptor;

typedef struct WGPUTextureViewDescriptor {
    const WGPUChainedStruct *nextInChain;
    WGPUStringView label;
    WGPUTextureFormat format;
    WGPUTextureViewDimension dimension;
    uint32_t baseMipLevel;
    uint32_t mipLevelCount;
    uint32_t baseArrayLayer;
    uint32_t arrayLayerCount;
    WGPUTextureAspect aspect;
} WGPUTextureViewDescriptor;

typedef struct WGPUShaderSourceWGSL {
    WGPUChainedStruct chain;
    WGPUStringView code;
} WGPUShaderSourceWGSL;

typedef struct WGPUShaderModuleDescriptor {
    const WGPUChainedStruct *nextInChain;
    WGPUStringView label;
} WGPUShaderModuleDescriptor;

typedef struct WGPUSamplerDescriptor {
    const WGPUChainedStruct *nextInChain;
    WGPUStringView label;
    WGPUAddressMode addressModeU;
    WGPUAddressMode addressModeV;
    WGPUAddressMode addressModeW;
    WGPUFilterMode magFilter;
    WGPUFilterMode minFilter;
    WGPUMipmapFilterMode mipmapFilter;
    float lodMinClamp;
    float lodMaxClamp;
    WGPUCompareFunction compare;
    uint16_t maxAnisotropy;
} WGPUSamplerDescriptor;

typedef struct WGPUCommandEncoderDescriptor {
    const WGPUChainedStruct *nextInChain;
    WGPUStringView label;
} WGPUCommandEncoderDescriptor;

typedef struct WGPUCommandBufferDescriptor {
    const WGPUChainedStruct *nextInChain;
    WGPUStringView label;
} WGPUCommandBufferDescriptor;

typedef struct WGPUBufferBindingLayout {
    WGPUBufferBindingType type;
    int hasDynamicOffset;
    uint64_t minBindingSize;
} WGPUBufferBindingLayout;

typedef struct WGPUSamplerBindingLayout {
    WGPUSamplerBindingType type;
} WGPUSamplerBindingLayout;

typedef struct WGPUTextureBindingLayout {
    WGPUTextureSampleType sampleType;
    WGPUTextureViewDimension viewDimension;
    int multisampled;
} WGPUTextureBindingLayout;

typedef struct WGPUStorageTextureBindingLayout {
    WGPUStorageTextureAccess access;
    WGPUTextureFormat format;
    WGPUTextureViewDimension viewDimension;
} WGPUStorageTextureBindingLayout;

typedef struct WGPUBindGroupLayoutEntry {
    const WGPUChainedStruct *nextInChain;
    uint32_t binding;
    WGPUShaderStage visibility;
    WGPUBufferBindingLayout buffer;
    WGPUSamplerBindingLayout sampler;
    WGPUTextureBindingLayout texture;
    WGPUStorageTextureBindingLayout storageTexture;
} WGPUBindGroupLayoutEntry;

typedef struct WGPUBindGroupLayoutDescriptor {
    const WGPUChainedStruct *nextInChain;
    WGPUStringView label;
    size_t entryCount;
    const WGPUBindGroupLayoutEntry *entries;
} WGPUBindGroupLayoutDescriptor;

typedef struct WGPUBindGroupEntry {
    const WGPUChainedStruct *nextInChain;
    uint32_t binding;
    WGPUBuffer buffer;
    uint64_t offset;
    uint64_t size;
    WGPUSampler sampler;
    WGPUTextureView textureView;
} WGPUBindGroupEntry;

typedef struct WGPUBindGroupDescriptor {
    const WGPUChainedStruct *nextInChain;
    WGPUStringView label;
    WGPUBindGroupLayout layout;
    size_t entryCount;
    const WGPUBindGroupEntry *entries;
} WGPUBindGroupDescriptor;

typedef struct WGPUPipelineLayoutDescriptor {
    const WGPUChainedStruct *nextInChain;
    WGPUStringView label;
    size_t bindGroupLayoutCount;
    const WGPUBindGroupLayout *bindGroupLayouts;
} WGPUPipelineLayoutDescriptor;

typedef struct WGPUVertexAttribute {
    WGPUVertexFormat format;
    uint64_t offset;
    uint32_t shaderLocation;
} WGPUVertexAttribute;

typedef struct WGPUVertexBufferLayout {
    uint64_t arrayStride;
    WGPUVertexStepMode stepMode;
    size_t attributeCount;
    const WGPUVertexAttribute *attributes;
} WGPUVertexBufferLayout;

typedef struct WGPUVertexState {
    const WGPUChainedStruct *nextInChain;
    WGPUShaderModule module;
    WGPUStringView entryPoint;
    size_t bufferCount;
    const WGPUVertexBufferLayout *buffers;
} WGPUVertexState;

typedef struct WGPUBlendComponent {
    uint32_t operation;
    uint32_t srcFactor;
    uint32_t dstFactor;
} WGPUBlendComponent;

typedef struct WGPUBlendState {
    WGPUBlendComponent color;
    WGPUBlendComponent alpha;
} WGPUBlendState;

typedef struct WGPUColorTargetState {
    const WGPUChainedStruct *nextInChain;
    WGPUTextureFormat format;
    const WGPUBlendState *blend;
    WGPUColorWriteMask writeMask;
} WGPUColorTargetState;

typedef struct WGPUFragmentState {
    const WGPUChainedStruct *nextInChain;
    WGPUShaderModule module;
    WGPUStringView entryPoint;
    size_t targetCount;
    const WGPUColorTargetState *targets;
} WGPUFragmentState;

typedef struct WGPUPrimitiveState {
    const WGPUChainedStruct *nextInChain;
    WGPUPrimitiveTopology topology;
    WGPUIndexFormat stripIndexFormat;
    WGPUFrontFace frontFace;
    WGPUCullMode cullMode;
} WGPUPrimitiveState;

typedef struct WGPUMultisampleState {
    const WGPUChainedStruct *nextInChain;
    uint32_t count;
    uint32_t mask;
    int alphaToCoverageEnabled;
} WGPUMultisampleState;

typedef struct WGPUDepthStencilState {
    const WGPUChainedStruct *nextInChain;
    WGPUTextureFormat format;
    int depthWriteEnabled;
    WGPUCompareFunction depthCompare;
} WGPUDepthStencilState;

typedef struct WGPURenderPipelineDescriptor {
    const WGPUChainedStruct *nextInChain;
    WGPUStringView label;
    WGPUPipelineLayout layout;
    WGPUVertexState vertex;
    WGPUPrimitiveState primitive;
    const WGPUDepthStencilState *depthStencil;
    WGPUMultisampleState multisample;
    const WGPUFragmentState *fragment;
} WGPURenderPipelineDescriptor;

typedef struct WGPUProgrammableStageDescriptor {
    const WGPUChainedStruct *nextInChain;
    WGPUShaderModule module;
    WGPUStringView entryPoint;
} WGPUProgrammableStageDescriptor;

typedef struct WGPUComputePipelineDescriptor {
    const WGPUChainedStruct *nextInChain;
    WGPUStringView label;
    WGPUPipelineLayout layout;
    WGPUProgrammableStageDescriptor compute;
} WGPUComputePipelineDescriptor;

typedef struct WGPURenderPassColorAttachment {
    const WGPUChainedStruct *nextInChain;
    WGPUTextureView view;
    uint32_t depthSlice;
    WGPUTextureView resolveTarget;
    WGPULoadOp loadOp;
    WGPUStoreOp storeOp;
    WGPUColor clearValue;
} WGPURenderPassColorAttachment;

typedef struct WGPURenderPassDepthStencilAttachment {
    WGPUTextureView view;
    WGPULoadOp depthLoadOp;
    WGPUStoreOp depthStoreOp;
    float depthClearValue;
    int depthReadOnly;
    WGPULoadOp stencilLoadOp;
    WGPUStoreOp stencilStoreOp;
    uint32_t stencilClearValue;
    int stencilReadOnly;
} WGPURenderPassDepthStencilAttachment;

typedef struct WGPURenderPassDescriptor {
    const WGPUChainedStruct *nextInChain;
    WGPUStringView label;
    size_t colorAttachmentCount;
    const WGPURenderPassColorAttachment *colorAttachments;
    const WGPURenderPassDepthStencilAttachment *depthStencilAttachment;
} WGPURenderPassDescriptor;

typedef struct WGPUTexelCopyBufferLayout {
    uint64_t offset;
    uint32_t bytesPerRow;
    uint32_t rowsPerImage;
} WGPUTexelCopyBufferLayout;

typedef struct WGPUTexelCopyBufferInfo {
    WGPUTexelCopyBufferLayout layout;
    WGPUBuffer buffer;
} WGPUTexelCopyBufferInfo;

typedef struct WGPUTexelCopyTextureInfo {
    WGPUTexture texture;
    uint32_t mipLevel;
    WGPUOrigin3D origin;
    WGPUTextureAspect aspect;
} WGPUTexelCopyTextureInfo;

typedef struct WGPUAdapterInfo {
    const char *vendor;
    const char *architecture;
    const char *device;
    const char *description;
} WGPUAdapterInfo;

/* ---- API function declarations ---- */
WGPUInstance wgpuCreateInstance(const WGPUInstanceDescriptor *descriptor);
void wgpuInstanceRelease(WGPUInstance instance);
void wgpuAdapterRelease(WGPUAdapter adapter);
WGPUDevice wgpuAdapterRequestDeviceSync(WGPUAdapter adapter,
                                        const WGPUDeviceDescriptor *descriptor);
void wgpuDeviceRelease(WGPUDevice device);
WGPUQueue wgpuDeviceGetQueue(WGPUDevice device);
void wgpuQueueRelease(WGPUQueue queue);
void wgpuQueueAddRef(WGPUQueue queue);
void wgpuQueueSubmit(WGPUQueue queue, size_t commandCount,
                     const WGPUCommandBuffer *commands);
void wgpuQueueWriteBuffer(WGPUQueue queue, WGPUBuffer buffer, uint64_t offset,
                          const void *data, size_t size);
WGPUBuffer wgpuDeviceCreateBuffer(WGPUDevice device,
                                  const WGPUBufferDescriptor *descriptor);
void wgpuBufferRelease(WGPUBuffer buffer);
WGPUTexture wgpuDeviceCreateTexture(WGPUDevice device,
                                    const WGPUTextureDescriptor *descriptor);
void wgpuTextureRelease(WGPUTexture texture);
WGPUTextureView wgpuTextureCreateView(WGPUTexture texture,
                                      const WGPUTextureViewDescriptor *descriptor);
void wgpuTextureViewRelease(WGPUTextureView view);
WGPUTextureFormat wgpuTextureGetFormat(WGPUTexture texture);
WGPUShaderModule wgpuDeviceCreateShaderModule(WGPUDevice device,
                                              const WGPUShaderModuleDescriptor *descriptor);
void wgpuShaderModuleRelease(WGPUShaderModule module);
WGPURenderPipeline wgpuDeviceCreateRenderPipeline(
    WGPUDevice device, const WGPURenderPipelineDescriptor *descriptor);
void wgpuRenderPipelineRelease(WGPURenderPipeline pipeline);
WGPUComputePipeline wgpuDeviceCreateComputePipeline(
    WGPUDevice device, const WGPUComputePipelineDescriptor *descriptor);
void wgpuComputePipelineRelease(WGPUComputePipeline pipeline);
WGPUPipelineLayout wgpuDeviceCreatePipelineLayout(
    WGPUDevice device, const WGPUPipelineLayoutDescriptor *descriptor);
void wgpuPipelineLayoutRelease(WGPUPipelineLayout layout);
WGPUBindGroupLayout wgpuDeviceCreateBindGroupLayout(
    WGPUDevice device, const WGPUBindGroupLayoutDescriptor *descriptor);
void wgpuBindGroupLayoutRelease(WGPUBindGroupLayout layout);
WGPUBindGroup wgpuDeviceCreateBindGroup(WGPUDevice device,
                                        const WGPUBindGroupDescriptor *descriptor);
void wgpuBindGroupRelease(WGPUBindGroup group);
WGPUSampler wgpuDeviceCreateSampler(WGPUDevice device,
                                    const WGPUSamplerDescriptor *descriptor);
void wgpuSamplerRelease(WGPUSampler sampler);
WGPUCommandEncoder wgpuDeviceCreateCommandEncoder(
    WGPUDevice device, const WGPUCommandEncoderDescriptor *descriptor);
void wgpuCommandEncoderRelease(WGPUCommandEncoder encoder);
WGPUCommandBuffer wgpuCommandEncoderFinish(WGPUCommandEncoder encoder,
                                           const WGPUCommandBufferDescriptor *descriptor);
void wgpuCommandBufferRelease(WGPUCommandBuffer buffer);
void wgpuCommandEncoderCopyBufferToBuffer(WGPUCommandEncoder encoder, WGPUBuffer src,
                                          uint64_t srcOffset, WGPUBuffer dst,
                                          uint64_t dstOffset, uint64_t size);
void wgpuCommandEncoderCopyBufferToTexture(WGPUCommandEncoder encoder,
                                           const WGPUTexelCopyBufferInfo *src,
                                           const WGPUTexelCopyTextureInfo *dst,
                                           const WGPUExtent3D *copySize);
void wgpuCommandEncoderCopyTextureToBuffer(WGPUCommandEncoder encoder,
                                           const WGPUTexelCopyTextureInfo *src,
                                           const WGPUTexelCopyBufferInfo *dst,
                                           const WGPUExtent3D *copySize);
WGPURenderPassEncoder wgpuCommandEncoderBeginRenderPass(
    WGPUCommandEncoder encoder, const WGPURenderPassDescriptor *descriptor);
void wgpuRenderPassEncoderEnd(WGPURenderPassEncoder encoder);
void wgpuRenderPassEncoderRelease(WGPURenderPassEncoder encoder);
void wgpuRenderPassEncoderSetPipeline(WGPURenderPassEncoder encoder,
                                      WGPURenderPipeline pipeline);
void wgpuRenderPassEncoderSetVertexBuffer(WGPURenderPassEncoder encoder, uint32_t slot,
                                          WGPUBuffer buffer, uint64_t offset,
                                          uint64_t size);
void wgpuRenderPassEncoderSetIndexBuffer(WGPURenderPassEncoder encoder, WGPUBuffer buffer,
                                         WGPUIndexFormat format, uint64_t offset,
                                         uint64_t size);
void wgpuRenderPassEncoderSetBindGroup(WGPURenderPassEncoder encoder, uint32_t groupIndex,
                                       WGPUBindGroup group, size_t dynamicOffsetCount,
                                       const uint32_t *dynamicOffsets);
void wgpuRenderPassEncoderDraw(WGPURenderPassEncoder encoder, uint32_t vertexCount,
                               uint32_t instanceCount, uint32_t firstVertex,
                               uint32_t firstInstance);
void wgpuRenderPassEncoderDrawIndexed(WGPURenderPassEncoder encoder, uint32_t indexCount,
                                      uint32_t instanceCount, uint32_t firstIndex,
                                      int32_t baseVertex, uint32_t firstInstance);
void wgpuRenderPassEncoderDrawIndirect(WGPURenderPassEncoder encoder, WGPUBuffer buffer,
                                       uint64_t offset);
void wgpuRenderPassEncoderDrawIndexedIndirect(WGPURenderPassEncoder encoder,
                                              WGPUBuffer buffer, uint64_t offset);
void wgpuRenderPassEncoderSetViewport(WGPURenderPassEncoder encoder, float x, float y,
                                      float width, float height, float minDepth,
                                      float maxDepth);
void wgpuRenderPassEncoderSetScissorRect(WGPURenderPassEncoder encoder, uint32_t x,
                                         uint32_t y, uint32_t width, uint32_t height);
void wgpuRenderPassEncoderSetBlendConstant(WGPURenderPassEncoder encoder,
                                           const WGPUColor *color);
void wgpuRenderPassEncoderSetStencilReference(WGPURenderPassEncoder encoder,
                                              uint32_t reference);
WGPUComputePassEncoder wgpuCommandEncoderBeginComputePass(
    WGPUCommandEncoder encoder, const void *descriptor);
void wgpuComputePassEncoderEnd(WGPUComputePassEncoder encoder);
void wgpuComputePassEncoderRelease(WGPUComputePassEncoder encoder);
void wgpuComputePassEncoderSetPipeline(WGPUComputePassEncoder encoder,
                                       WGPUComputePipeline pipeline);
void wgpuComputePassEncoderSetBindGroup(WGPUComputePassEncoder encoder, uint32_t groupIndex,
                                        WGPUBindGroup group, size_t dynamicOffsetCount,
                                        const uint32_t *dynamicOffsets);
void wgpuComputePassEncoderDispatchWorkgroups(WGPUComputePassEncoder encoder, uint32_t x,
                                              uint32_t y, uint32_t z);
void wgpuComputePassEncoderDispatchWorkgroupsIndirect(WGPUComputePassEncoder encoder,
                                                      WGPUBuffer buffer, uint64_t offset);

#ifdef __cplusplus
}
#endif

#endif /* WEBGPU_H_ */
