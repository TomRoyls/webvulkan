#include <webgpu/webgpu.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <stdio.h>
#include <stdlib.h>

static WGPUDevice device = NULL;
static WGPUSurface surface = NULL;
static WGPUTextureFormat surface_format;
static WGPURenderPipeline pipeline = NULL;
static WGPUQueue queue = NULL;

static const char* vertex_shader_wgsl =
    "@vertex fn main(@builtin(vertex_index) idx: u32) -> @builtin(position) vec4f {\n"
    "    var pos = array<vec2f, 3>(vec2f(0.0, -0.7), vec2f(-0.7, 0.7), vec2f(0.7, 0.7));\n"
    "    return vec4f(pos[idx], 0.0, 1.0);\n"
    "}\n";

static const char* fragment_shader_wgsl =
    "@fragment fn main(@builtin(position) pos: vec4f) -> @location(0) vec4f {\n"
    "    return vec4f(1.0, 0.5, 0.0, 1.0);\n"
    "}\n";

static WGPUShaderModule create_shader(WGPUDevice dev, const char* wgsl) {
    WGPUShaderSourceWGSL wgsl_desc = {
        .chain = {
            .next = NULL,
            .sType = WGPUSType_ShaderSourceWGSL,
        },
        .code = (WGPUStringView){ .data = wgsl, .length = WGPU_STRLEN },
    };
    
    WGPUShaderModuleDescriptor desc = {
        .nextInChain = (const WGPUChainedStruct*)&wgsl_desc,
        .label = (WGPUStringView){ .data = NULL, .length = WGPU_STRLEN },
    };
    
    return wgpuDeviceCreateShaderModule(dev, &desc);
}

static void init_pipeline(void) {
    WGPUShaderModule vs = create_shader(device, vertex_shader_wgsl);
    WGPUShaderModule fs = create_shader(device, fragment_shader_wgsl);
    
    WGPUBlendState blend = {
        .color = {
            .operation = WGPUBlendOperation_Add,
            .srcFactor = WGPUBlendFactor_One,
            .dstFactor = WGPUBlendFactor_Zero,
        },
        .alpha = {
            .operation = WGPUBlendOperation_Add,
            .srcFactor = WGPUBlendFactor_One,
            .dstFactor = WGPUBlendFactor_Zero,
        },
    };
    
    WGPUColorTargetState color_target = {
        .format = surface_format,
        .blend = &blend,
        .writeMask = WGPUColorWriteMask_All,
    };
    
    WGPUFragmentState fragment = {
        .module = fs,
        .entryPoint = (WGPUStringView){ .data = "main", .length = WGPU_STRLEN },
        .constantCount = 0,
        .constants = NULL,
        .targetCount = 1,
        .targets = &color_target,
    };
    
    WGPURenderPipelineDescriptor desc = {
        .label = (WGPUStringView){ .data = "TrianglePipeline", .length = WGPU_STRLEN },
        .layout = NULL,
        .vertex = {
            .module = vs,
            .entryPoint = (WGPUStringView){ .data = "main", .length = WGPU_STRLEN },
            .constantCount = 0,
            .constants = NULL,
            .bufferCount = 0,
            .buffers = NULL,
        },
        .primitive = {
            .topology = WGPUPrimitiveTopology_TriangleList,
            .stripIndexFormat = WGPUIndexFormat_Undefined,
            .frontFace = WGPUFrontFace_CCW,
            .cullMode = WGPUCullMode_None,
            .unclippedDepth = false,
        },
        .fragment = &fragment,
        .depthStencil = NULL,
        .multisample = {
            .count = 1,
            .mask = 0xFFFFFFFF,
            .alphaToCoverageEnabled = false,
        },
    };
    
    pipeline = wgpuDeviceCreateRenderPipeline(device, &desc);
    
    wgpuShaderModuleRelease(vs);
    wgpuShaderModuleRelease(fs);
}

static void frame(void* arg) {
    (void)arg;
    
    WGPUSurfaceTexture surface_texture;
    wgpuSurfaceGetCurrentTexture(surface, &surface_texture);
    
    if (surface_texture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal &&
        surface_texture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessSuboptimal) {
        printf("Surface texture error: %d\n", surface_texture.status);
        return;
    }
    
    WGPUTextureViewDescriptor view_desc = {
        .label = (WGPUStringView){ .data = NULL, .length = WGPU_STRLEN },
        .format = surface_format,
        .dimension = WGPUTextureViewDimension_2D,
        .aspect = WGPUTextureAspect_All,
        .baseMipLevel = 0,
        .mipLevelCount = 1,
        .baseArrayLayer = 0,
        .arrayLayerCount = 1,
    };
    
    WGPUTextureView backbuffer = wgpuTextureCreateView(surface_texture.texture, &view_desc);
    
    WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device, NULL);
    
    WGPURenderPassColorAttachment color_attachment = {
        .view = backbuffer,
        .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
        .resolveTarget = NULL,
        .loadOp = WGPULoadOp_Clear,
        .storeOp = WGPUStoreOp_Store,
        .clearValue = {0.05, 0.05, 0.1, 1.0},
    };
    
    WGPURenderPassDescriptor render_pass = {
        .label = (WGPUStringView){ .data = NULL, .length = WGPU_STRLEN },
        .colorAttachmentCount = 1,
        .colorAttachments = &color_attachment,
        .depthStencilAttachment = NULL,
        .occlusionQuerySet = NULL,
        .timestampWrites = NULL,
    };
    
    WGPURenderPassEncoder pass = wgpuCommandEncoderBeginRenderPass(encoder, &render_pass);
    
    wgpuRenderPassEncoderSetPipeline(pass, pipeline);
    wgpuRenderPassEncoderDraw(pass, 3, 1, 0, 0);
    wgpuRenderPassEncoderEnd(pass);
    wgpuRenderPassEncoderRelease(pass);
    
    WGPUCommandBuffer cmd = wgpuCommandEncoderFinish(encoder, NULL);
    wgpuCommandEncoderRelease(encoder);
    
    wgpuQueueSubmit(queue, 1, &cmd);
    wgpuCommandBufferRelease(cmd);
    
    wgpuSurfacePresent(surface);
    wgpuTextureViewRelease(backbuffer);
    wgpuTextureRelease(surface_texture.texture);
}

static void configure_surface(int width, int height) {
    WGPUSurfaceConfiguration config = {
        .nextInChain = NULL,
        .device = device,
        .format = surface_format,
        .usage = WGPUTextureUsage_RenderAttachment,
        .width = width,
        .height = height,
        .presentMode = WGPUPresentMode_Fifo,
        .alphaMode = WGPUCompositeAlphaMode_Auto,
        .viewFormatCount = 0,
        .viewFormats = NULL,
    };
    wgpuSurfaceConfigure(surface, &config);
}

int main(void) {
    printf("WebVulkan Triangle - WebGPU Backend\n");
    
    device = emscripten_webgpu_get_device();
    if (!device) {
        printf("ERROR: Failed to get WebGPU device\n");
        return 1;
    }
    printf("Got WebGPU device\n");
    
    queue = wgpuDeviceGetQueue(device);
    
    WGPUInstance instance = wgpuCreateInstance(NULL);
    
    WGPUEmscriptenSurfaceSourceCanvasHTMLSelector canvas_selector = {
        .chain = {
            .next = NULL,
            .sType = WGPUSType_EmscriptenSurfaceSourceCanvasHTMLSelector,
        },
        .selector = "#canvas",
    };
    
    WGPUSurfaceDescriptor surface_desc = {
        .nextInChain = (const WGPUChainedStruct*)&canvas_selector,
        .label = (WGPUStringView){ .data = NULL, .length = WGPU_STRLEN },
    };
    
    surface = wgpuInstanceCreateSurface(instance, &surface_desc);
    printf("Created surface\n");
    
    WGPUSurfaceCapabilities caps;
    wgpuSurfaceGetCapabilities(surface, NULL, &caps);
    surface_format = caps.formats[0];
    for (uint32_t i = 0; i < caps.formatCount; i++) {
        if (caps.formats[i] == WGPUTextureFormat_BGRA8Unorm ||
            caps.formats[i] == WGPUTextureFormat_RGBA8Unorm) {
            surface_format = caps.formats[i];
            break;
        }
    }
    wgpuSurfaceCapabilitiesFreeMembers(caps);
    printf("Surface format: %d\n", surface_format);
    
    configure_surface(800, 600);
    
    init_pipeline();
    printf("Pipeline created\n");
    
    emscripten_set_main_loop_arg(frame, NULL, 0, 1);
    
    return 0;
}
