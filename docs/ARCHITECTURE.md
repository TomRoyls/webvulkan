# WebVulkan Architecture

## Overview

WebVulkan is a translation layer that implements the Vulkan API on top of WebGPU. It enables Vulkan applications to run in WebAssembly-enabled browsers by translating Vulkan calls to their WebGPU equivalents at runtime.

```
┌─────────────────────────────────────────────────────────────┐
│                     Application Layer                        │
│  (Uses standard Vulkan API - vkCreateInstance, vkDraw, etc) │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                     WebVulkan Library                        │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐  │
│  │    Core     │  │   Objects   │  │   Shader Transpile  │  │
│  │ Instance    │  │ Buffer      │  │  SPIR-V → WGSL      │  │
│  │ Device      │  │ Image       │  │                     │  │
│  │ Queue       │  │ Pipeline    │  │                     │  │
│  └─────────────┘  └─────────────┘  └─────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                     WebGPU (Browser)                         │
│            WGPUInstance, WGPUDevice, WGPUQueue              │
└─────────────────────────────────────────────────────────────┘
```

## Module Structure

### Core (`src/core/`)

Handles instance and device creation - the entry points for any Vulkan application.

| File | Purpose |
|------|---------|
| `instance.c` | VkInstance implementation, creates WGPUInstance |
| `device.c` | VkDevice implementation, creates WGPUDevice |
| `physical_device.c` | VkPhysicalDevice, adapter enumeration |
| `queue.c` | VkQueue, command submission |

**Object Mapping:**
```c
struct VkInstance_T {
    WGPUInstance wgpu_instance;
    VkPhysicalDevice phys_dev;  // cached
};

struct VkDevice_T {
    WGPUDevice wgpu_device;
    WGPUQueue wgpu_queue;
    WGPUBuffer push_constant_buffer;
};
```

### Objects (`src/objects/`)

Implements Vulkan object types that map to WebGPU resources.

| File | Vulkan Type | WebGPU Type |
|------|-------------|-------------|
| `buffer.c` | VkBuffer | WGPUBuffer |
| `image.c` | VkImage | WGPUTexture |
| `image_view.c` | VkImageView | WGPUTextureView |
| `sampler.c` | VkSampler | WGPUSampler |
| `shader_module.c` | VkShaderModule | WGPUShaderModule |
| `pipeline.c` | VkPipeline | WGPURenderPipeline / WGPUComputePipeline |
| `pipeline_layout.c` | VkPipelineLayout | WGPUPipelineLayout |
| `descriptor_set.c` | VkDescriptorSet | WGPUBindGroup |
| `descriptor_set_layout.c` | VkDescriptorSetLayout | WGPUBindGroupLayout |
| `render_pass.c` | VkRenderPass | (description cache) |
| `framebuffer.c` | VkFramebuffer | (attachment cache) |
| `command_pool.c` | VkCommandPool | (allocator) |
| `command_buffer.c` | VkCommandBuffer | WGPUCommandEncoder |
| `semaphore.c` | VkSemaphore | (emulated) |
| `fence.c` | VkFence | (emulated) |
| `event.c` | VkEvent | (emulated) |

### Commands (`src/commands/`)

Records commands into command buffers for later execution.

| File | Purpose |
|------|---------|
| `draw.c` | vkCmdDraw, vkCmdDrawIndexed |
| `compute.c` | vkCmdDispatch |
| `copy.c` | vkCmdCopyBuffer, vkCmdCopyImage |
| `sync.c` | vkCmdPipelineBarrier |
| `render_pass.c` | vkCmdBeginRenderPass, vkCmdEndRenderPass |
| `subpass.c` | Subpass management |

### Shaders (`src/shaders/`)

Transpiles SPIR-V shaders to WGSL for WebGPU consumption.

| File | Purpose |
|------|---------|
| `spirv_parser.c` | Parses SPIR-V binary into intermediate representation |
| `spirv_parser.h` | SPIR-V opcode definitions and data structures |
| `wgsl_gen.c` | Generates WGSL from parsed SPIR-V |
| `wgsl_gen.h` | WGSL generator interface |

**Transpilation Flow:**
```
SPIR-V Binary → spirv_parser.c → WgvkSpvModule (IR)
                                    ↓
                              wgsl_gen.c → WGSL Source
                                    ↓
                            WGPUShaderModule
```

### Sync (`src/sync/`)

Synchronization primitives between CPU and GPU.

| File | Purpose |
|------|---------|
| `barrier.c` | Memory barriers, pipeline barriers |
| `push_constants.c` | Push constant handling via uniform buffer |

### Memory (`src/memory/`)

GPU memory management.

| File | Purpose |
|------|---------|
| `device_memory.c` | VkDeviceMemory, memory allocation and mapping |

### Utilities (`src/util/`)

Common data structures and helpers.

| File | Purpose |
|------|---------|
| `list.c` | Intrusive linked list |
| `hash_table.c` | Hash table for object lookup |
| `log.c` | Logging with levels and categories |

## Object Lifecycle

All Vulkan objects use reference counting via the `WgvkObject` base:

```c
struct WgvkObject {
    volatile int32_t ref_count;
    void (*destroy)(void* obj);
};

void wgvk_object_init(struct WgvkObject* obj, void (*destroy)(void*));
void wgvk_object_retain(struct WgvkObject* obj);
void wgvk_object_release(struct WgvkObject* obj);
```

**Pattern:**
1. `vkCreate*` allocates object, sets ref_count = 1
2. `vkDestroy*` calls `wgvk_object_release`
3. When ref_count reaches 0, destroy callback is invoked
4. Destroy callback frees memory and releases child objects

## Shader Transpilation

### Current Limitations

The SPIR-V to WGSL transpiler currently generates stub output:
- Parses SPIR-V structure (types, constants, variables, decorations)
- Emits correct WGSL struct definitions and entry point signatures
- **Does NOT translate shader logic** (arithmetic, control flow)

### Recommended Approach

For production use, pass WGSL source directly to `vkCreateShaderModule`:

```c
VkShaderModuleCreateInfo info = {
    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    .codeSize = strlen(wgsl_source),
    .pCode = (const uint32_t*)wgsl_source,
};
```

### Future Work

Full transpilation requires:
1. Extended SPIR-V instruction decoding (all arithmetic, texture ops)
2. Control flow graph construction
3. SSA to WGSL variable conversion
4. WGSL-specific type handling (no pointers in function params)

## Threading Model

- WebVulkan is **not thread-safe**
- All calls must come from a single thread
- This matches WebGPU's single-threaded JavaScript model

## Memory Management

- Uses system `calloc`/`free` (no custom allocators)
- `VkAllocationCallbacks` parameter is ignored
- GPU memory is backed by WebGPU buffers/textures

## API Conventions

- Vulkan handles are opaque pointers to internal structs
- Return `VK_SUCCESS` on success, appropriate error code on failure
- Validation is minimal - prefer correct usage over defensive checks

## Building for WebAssembly

Key CMake flags:
```cmake
-DWEBVULKAN_WASM=ON           # Enable Emscripten build
-DWEBVULKAN_BUILD_TESTS=OFF   # Tests not supported in WASM
-DWEBVULKAN_BUILD_SAMPLES=ON  # Build triangle demo
```

Emscripten flags:
```
--use-port=emdawnwebgpu       # WebGPU port
-sALLOW_MEMORY_GROWTH=1       # Dynamic memory
-sENVIRONMENT=web             # Web target
```
