# WebVulkan

A C11 library that implements a Vulkan-to-WebGPU translation layer, enabling Vulkan API code to run in browsers via WebAssembly.

## Overview

WebVulkan acts as a shim between the Vulkan API and WebGPU, translating Vulkan calls to their WebGPU equivalents at runtime. This allows existing Vulkan codebases to target the web with minimal modifications.

```
┌─────────────────┐
│  Application    │
│  (Vulkan API)   │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│   WebVulkan     │  ← Translation Layer
│  (this library) │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│     WebGPU      │
│  (Browser/GPU)  │
└─────────────────┘
```

## Features

- **Vulkan API Compatibility**: Implements core Vulkan 1.0 API subset
- **SPIR-V to WGSL Transpilation**: Converts Vulkan shaders to WebGPU-compatible WGSL
- **WebAssembly Support**: Builds with Emscripten for browser deployment
- **Zero Dependencies**: Only requires Vulkan headers (fetched automatically)
- **Reference Counting**: Proper object lifecycle management

## Building

### Native Build (Linux/macOS)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### WebAssembly Build

Requires [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html).

```bash
# Setup Emscripten
source /path/to/emsdk/emsdk_env.sh

# Configure and build
emcmake cmake -B build-wasm \
    -DCMAKE_BUILD_TYPE=Release \
    -DWEBVULKAN_WASM=ON \
    -DWEBVULKAN_BUILD_TESTS=OFF \
    -DWEBVULKAN_BUILD_SAMPLES=ON

cmake --build build-wasm
```

### Build Options

| Option | Default | Description |
|--------|---------|-------------|
| `WEBVULKAN_BUILD_SAMPLES` | ON | Build sample applications |
| `WEBVULKAN_BUILD_TESTS` | ON | Build unit tests |
| `WEBVULKAN_WASM` | OFF | Build for WebAssembly |

## Usage

### Basic Instance Creation

```c
#include <vulkan/vulkan.h>

VkApplicationInfo app_info = {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pApplicationName = "MyApp",
    .apiVersion = VK_API_VERSION_1_0,
};

VkInstanceCreateInfo create_info = {
    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .pApplicationInfo = &app_info,
};

VkInstance instance;
VkResult result = vkCreateInstance(&create_info, NULL, &instance);
```

### Shader Creation (Direct WGSL)

WebVulkan supports direct WGSL shader input by passing the WGSL source as the shader code:

```c
const char* vertex_wgsl = 
    "@vertex\n"
    "fn main(@location(0) pos: vec2<f32>) -> @builtin(position) vec4<f32> {\n"
    "    return vec4<f32>(pos, 0.0, 1.0);\n"
    "}\n";

VkShaderModuleCreateInfo shader_info = {
    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    .codeSize = strlen(vertex_wgsl),
    .pCode = (const uint32_t*)vertex_wgsl,
};

VkShaderModule module;
vkCreateShaderModule(device, &shader_info, NULL, &module);
```

### Running Samples

After building for WebAssembly, serve the sample files:

```bash
cd build-wasm/samples
python3 -m http.server 8080
```

Then open `http://localhost:8080/triangle/` in a WebGPU-compatible browser.

## API Coverage

### Core Objects

| Object | Status | Notes |
|--------|--------|-------|
| VkInstance | ✅ | Full support |
| VkPhysicalDevice | ✅ | Single device |
| VkDevice | ✅ | Full support |
| VkQueue | ✅ | Full support |
| VkCommandBuffer | ✅ | Basic recording |

### Resource Objects

| Object | Status | Notes |
|--------|--------|-------|
| VkBuffer | ✅ | Full support |
| VkImage | ✅ | 2D textures |
| VkImageView | ✅ | Full support |
| VkDeviceMemory | ✅ | Host-visible memory |
| VkSampler | ✅ | Basic sampling |

### Pipeline Objects

| Object | Status | Notes |
|--------|--------|-------|
| VkShaderModule | ✅ | WGSL input |
| VkPipelineLayout | ✅ | Full support |
| VkPipeline | ✅ | Graphics pipelines |
| VkDescriptorSet | ✅ | Uniform buffers |
| VkDescriptorSetLayout | ✅ | Full support |

### Synchronization

| Object | Status | Notes |
|--------|--------|-------|
| VkSemaphore | ✅ | Binary semaphores |
| VkFence | ✅ | Full support |
| VkEvent | ✅ | Full support |

## Architecture

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for detailed architecture documentation.

### Module Structure

```
src/
├── core/           # Instance, Device, PhysicalDevice, Queue
├── objects/        # Vulkan object implementations
├── commands/       # Command buffer recording
├── shaders/        # SPIR-V parser and WGSL generator
├── sync/           # Synchronization primitives
├── memory/         # Memory management
└── util/           # Utilities (list, hash table)
```

## Browser Compatibility

WebVulkan requires WebGPU support. Tested browsers:

- Chrome 113+ (stable)
- Chrome Canary (latest)
- Edge 113+
- Firefox Nightly (behind flag)

## Limitations

- Single physical device only
- No tessellation or geometry shaders
- Limited SPIR-V transpilation (direct WGSL recommended)
- No ray tracing support

## Contributing

1. Fork the repository
2. Create a feature branch
3. Ensure code passes `clang-format` and `clang-tidy`
4. Add tests for new functionality
5. Submit a pull request

## License

MIT License - see LICENSE file for details.

## Acknowledgments

- [Khronos Group](https://www.khronos.org/) for Vulkan and WebGPU specifications
- [Emscripten](https://emscripten.org/) for WebAssembly toolchain
- [Google Naga](https://github.com/gfx-rs/naga) for SPIR-V/WGSL translation reference
