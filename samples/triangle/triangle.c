#include "vulkan_platform.h"
#include <vulkan/vulkan.h>
#include "wgvk_accessors.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <webgpu/webgpu.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CANVAS_WIDTH 800
#define CANVAS_HEIGHT 600

static VkInstance g_instance = VK_NULL_HANDLE;
static VkPhysicalDevice g_physical_device = VK_NULL_HANDLE;
static VkDevice g_device = VK_NULL_HANDLE;
static VkQueue g_queue = VK_NULL_HANDLE;
static VkCommandPool g_command_pool = VK_NULL_HANDLE;
static VkBuffer g_vertex_buffer = VK_NULL_HANDLE;
static VkDeviceMemory g_vertex_memory = VK_NULL_HANDLE;
static VkShaderModule g_vertex_shader = VK_NULL_HANDLE;
static VkShaderModule g_fragment_shader = VK_NULL_HANDLE;
static VkPipelineLayout g_pipeline_layout = VK_NULL_HANDLE;
static VkPipeline g_pipeline = VK_NULL_HANDLE;

#ifdef __EMSCRIPTEN__
static WGPUSurface g_surface = NULL;
static WGPUTextureFormat g_surface_format;
#endif

static const float g_vertices[] = {
     0.0f, -0.7f,
    -0.7f,  0.7f,
     0.7f,  0.7f,
};

static const char g_vertex_wgsl[] =
    "@vertex\n"
    "fn main(@location(0) in_pos: vec2<f32>) -> @builtin(position) vec4<f32> {\n"
    "    return vec4<f32>(in_pos, 0.0, 1.0);\n"
    "}\n";

static const char g_fragment_wgsl[] =
    "@fragment\n"
    "fn main() -> @location(0) vec4<f32> {\n"
    "    return vec4<f32>(0.2, 0.6, 1.0, 1.0);\n"
    "}\n";

static VkResult create_shader_from_wgsl(VkDevice device, const char* wgsl, VkShaderModule* out_module) {
    VkShaderModuleCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = strlen(wgsl),
        .pCode = (const uint32_t*)wgsl,
    };
    return vkCreateShaderModule(device, &info, NULL, out_module);
}

static VkResult init_vulkan(void) {
    printf("[webvulkan] Creating Vulkan instance...\n");
    
    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Triangle",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "WebVulkan",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_0,
    };
    
    VkInstanceCreateInfo instance_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app_info,
    };
    
    VkResult result = vkCreateInstance(&instance_info, NULL, &g_instance);
    if (result != VK_SUCCESS) { printf("[webvulkan] vkCreateInstance failed: %d\n", result); return result; }
    printf("[webvulkan] Instance created\n");
    
    uint32_t device_count = 1;
    result = vkEnumeratePhysicalDevices(g_instance, &device_count, &g_physical_device);
    if (result != VK_SUCCESS || device_count == 0) { return VK_ERROR_INITIALIZATION_FAILED; }
    printf("[webvulkan] Physical device selected\n");
    
    float queue_priority = 1.0f;
    VkDeviceQueueCreateInfo queue_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = 0,
        .queueCount = 1,
        .pQueuePriorities = &queue_priority,
    };
    
    VkDeviceCreateInfo device_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queue_info,
    };
    
    result = vkCreateDevice(g_physical_device, &device_info, NULL, &g_device);
    if (result != VK_SUCCESS) { printf("[webvulkan] vkCreateDevice failed: %d\n", result); return result; }
    printf("[webvulkan] Device created\n");
    
    vkGetDeviceQueue(g_device, 0, 0, &g_queue);
    
    VkBufferCreateInfo buffer_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = sizeof(g_vertices),
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };
    result = vkCreateBuffer(g_device, &buffer_info, NULL, &g_vertex_buffer);
    if (result != VK_SUCCESS) return result;
    
    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(g_device, g_vertex_buffer, &mem_reqs);
    
    VkMemoryAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = mem_reqs.size,
        .memoryTypeIndex = 0,
    };
    result = vkAllocateMemory(g_device, &alloc_info, NULL, &g_vertex_memory);
    if (result != VK_SUCCESS) return result;
    
    void* mapped = NULL;
    result = vkMapMemory(g_device, g_vertex_memory, 0, sizeof(g_vertices), 0, &mapped);
    if (result == VK_SUCCESS && mapped) {
        memcpy(mapped, g_vertices, sizeof(g_vertices));
        vkUnmapMemory(g_device, g_vertex_memory);
    }
    vkBindBufferMemory(g_device, g_vertex_buffer, g_vertex_memory, 0);
    printf("[webvulkan] Vertex buffer ready\n");
    
    result = create_shader_from_wgsl(g_device, g_vertex_wgsl, &g_vertex_shader);
    if (result != VK_SUCCESS) { printf("[webvulkan] Vertex shader failed: %d\n", result); return result; }
    
    result = create_shader_from_wgsl(g_device, g_fragment_wgsl, &g_fragment_shader);
    if (result != VK_SUCCESS) { printf("[webvulkan] Fragment shader failed: %d\n", result); return result; }
    printf("[webvulkan] Shaders created\n");
    
    VkPipelineLayoutCreateInfo layout_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    };
    result = vkCreatePipelineLayout(g_device, &layout_info, NULL, &g_pipeline_layout);
    if (result != VK_SUCCESS) return result;
    
    VkVertexInputBindingDescription binding = {
        .binding = 0,
        .stride = 2 * sizeof(float),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };
    VkVertexInputAttributeDescription attr = {
        .binding = 0,
        .location = 0,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = 0,
    };
    VkPipelineVertexInputStateCreateInfo vertex_input = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &binding,
        .vertexAttributeDescriptionCount = 1,
        .pVertexAttributeDescriptions = &attr,
    };
    VkPipelineInputAssemblyStateCreateInfo input_assembly = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };
    VkPipelineRasterizationStateCreateInfo raster = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_NONE,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .lineWidth = 1.0f,
    };
    VkPipelineColorBlendAttachmentState blend_attachment = {
        .blendEnable = VK_FALSE,
        .colorWriteMask = 0xF,
    };
    VkPipelineColorBlendStateCreateInfo blend = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &blend_attachment,
    };
    
    VkPipelineShaderStageCreateInfo stages[2] = {
        { .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, .stage = VK_SHADER_STAGE_VERTEX_BIT, .module = g_vertex_shader, .pName = "main" },
        { .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, .stage = VK_SHADER_STAGE_FRAGMENT_BIT, .module = g_fragment_shader, .pName = "main" },
    };
    
    VkGraphicsPipelineCreateInfo pipeline_info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = stages,
        .pVertexInputState = &vertex_input,
        .pInputAssemblyState = &input_assembly,
        .pRasterizationState = &raster,
        .pColorBlendState = &blend,
        .layout = g_pipeline_layout,
    };
    
    result = vkCreateGraphicsPipelines(g_device, VK_NULL_HANDLE, 1, &pipeline_info, NULL, &g_pipeline);
    if (result != VK_SUCCESS) { printf("[webvulkan] Pipeline creation failed: %d\n", result); return result; }
    printf("[webvulkan] Pipeline created\n");
    
    printf("[webvulkan] Vulkan initialization complete\n");
    return VK_SUCCESS;
}

#ifdef __EMSCRIPTEN__

static void init_webgpu_surface(void) {
    WGPUInstance instance = wgpuCreateInstance(NULL);
    
    WGPUEmscriptenSurfaceSourceCanvasHTMLSelector canvas_selector = {
        .chain = { .next = NULL, .sType = WGPUSType_EmscriptenSurfaceSourceCanvasHTMLSelector },
        .selector = "#canvas",
    };
    WGPUSurfaceDescriptor surface_desc = { .nextInChain = (const WGPUChainedStruct*)&canvas_selector };
    g_surface = wgpuInstanceCreateSurface(instance, &surface_desc);
    
    WGPUSurfaceCapabilities caps;
    wgpuSurfaceGetCapabilities(g_surface, NULL, &caps);
    g_surface_format = caps.formats[0];
    for (uint32_t i = 0; i < caps.formatCount; i++) {
        if (caps.formats[i] == WGPUTextureFormat_BGRA8Unorm || caps.formats[i] == WGPUTextureFormat_RGBA8Unorm) {
            g_surface_format = caps.formats[i];
            break;
        }
    }
    wgpuSurfaceCapabilitiesFreeMembers(caps);
    
    WGPUSurfaceConfiguration config = {
        .device = emscripten_webgpu_get_device(),
        .format = g_surface_format,
        .usage = WGPUTextureUsage_RenderAttachment,
        .width = CANVAS_WIDTH,
        .height = CANVAS_HEIGHT,
        .presentMode = WGPUPresentMode_Fifo,
        .alphaMode = WGPUCompositeAlphaMode_Auto,
    };
    wgpuSurfaceConfigure(g_surface, &config);
    printf("[webvulkan] Surface configured, format=%d\n", g_surface_format);
}

static void render_frame(void) {
    WGPUSurfaceTexture surface_texture;
    wgpuSurfaceGetCurrentTexture(g_surface, &surface_texture);
    if (surface_texture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal &&
        surface_texture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessSuboptimal) return;
    
    WGPUTextureViewDescriptor view_desc = {
        .format = g_surface_format,
        .dimension = WGPUTextureViewDimension_2D,
        .aspect = WGPUTextureAspect_All,
        .baseMipLevel = 0, .mipLevelCount = 1,
        .baseArrayLayer = 0, .arrayLayerCount = 1,
    };
    WGPUTextureView backbuffer = wgpuTextureCreateView(surface_texture.texture, &view_desc);
    
    WGPUDevice device = emscripten_webgpu_get_device();
    WGPUQueue queue = wgpuDeviceGetQueue(device);
    WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device, NULL);
    
    WGPURenderPassColorAttachment color_attachment = {
        .view = backbuffer,
        .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
        .loadOp = WGPULoadOp_Clear,
        .storeOp = WGPUStoreOp_Store,
        .clearValue = {0.05, 0.05, 0.1, 1.0},
    };
    WGPURenderPassDescriptor render_pass_desc = {
        .colorAttachmentCount = 1,
        .colorAttachments = &color_attachment,
    };
    
    WGPURenderPassEncoder pass = wgpuCommandEncoderBeginRenderPass(encoder, &render_pass_desc);
    
    WGPURenderPipeline render_pipeline = wgvk_pipeline_get_render(g_pipeline);
    if (render_pipeline) {
        wgpuRenderPassEncoderSetPipeline(pass, render_pipeline);
        WGPUBuffer vertex_buf = wgvk_buffer_get_wgpu(g_vertex_buffer);
        if (vertex_buf) {
            wgpuRenderPassEncoderSetVertexBuffer(pass, 0, vertex_buf, 0, sizeof(g_vertices));
        }
        wgpuRenderPassEncoderDraw(pass, 3, 1, 0, 0);
    }
    
    wgpuRenderPassEncoderEnd(pass);
    wgpuRenderPassEncoderRelease(pass);
    
    WGPUCommandBuffer cmd = wgpuCommandEncoderFinish(encoder, NULL);
    wgpuCommandEncoderRelease(encoder);
    wgpuQueueSubmit(queue, 1, &cmd);
    wgpuCommandBufferRelease(cmd);
    
    wgpuSurfacePresent(g_surface);
    wgpuTextureViewRelease(backbuffer);
    wgpuTextureRelease(surface_texture.texture);
}

#endif

static void cleanup(void) {
    if (g_device) {
        vkDeviceWaitIdle(g_device);
        if (g_pipeline) vkDestroyPipeline(g_device, g_pipeline, NULL);
        if (g_pipeline_layout) vkDestroyPipelineLayout(g_device, g_pipeline_layout, NULL);
        if (g_fragment_shader) vkDestroyShaderModule(g_device, g_fragment_shader, NULL);
        if (g_vertex_shader) vkDestroyShaderModule(g_device, g_vertex_shader, NULL);
        if (g_vertex_buffer) vkDestroyBuffer(g_device, g_vertex_buffer, NULL);
        if (g_vertex_memory) vkFreeMemory(g_device, g_vertex_memory, NULL);
        vkDestroyDevice(g_device, NULL);
    }
    if (g_instance) vkDestroyInstance(g_instance, NULL);
}

int main(void) {
    printf("========================================\n");
    printf("WebVulkan Triangle Demo\n");
    printf("Vulkan API -> webvulkan -> WebGPU\n");
    printf("========================================\n\n");
    
    VkResult result = init_vulkan();
    if (result != VK_SUCCESS) {
        printf("[webvulkan] Initialization failed: %d\n", result);
        cleanup();
        return 1;
    }
    
#ifdef __EMSCRIPTEN__
    printf("[webvulkan] Initializing WebGPU surface...\n");
    init_webgpu_surface();
    printf("[webvulkan] Starting render loop\n");
    emscripten_set_main_loop(render_frame, 0, 1);
#else
    printf("[webvulkan] Vulkan initialized successfully (native build)\n");
    cleanup();
#endif
    
    return 0;
}
