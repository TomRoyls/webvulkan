#include <webvulkan.h>

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
static VkCommandBuffer g_command_buffer = VK_NULL_HANDLE;
static VkBuffer g_vertex_buffer = VK_NULL_HANDLE;
static VkDeviceMemory g_vertex_memory = VK_NULL_HANDLE;

#ifdef __EMSCRIPTEN__
static WGPUSurface g_surface = NULL;
static WGPUTextureFormat g_surface_format;
#endif

static const float g_vertices[] = {
     0.0f, -0.7f, 0.0f,
    -0.7f,  0.7f, 0.0f,
     0.7f,  0.7f, 0.0f,
};

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
    if (result != VK_SUCCESS) {
        printf("[webvulkan] vkCreateInstance failed: %d\n", result);
        return result;
    }
    printf("[webvulkan] Instance created\n");
    
    uint32_t device_count = 1;
    result = vkEnumeratePhysicalDevices(g_instance, &device_count, &g_physical_device);
    if (result != VK_SUCCESS || device_count == 0) {
        printf("[webvulkan] No physical devices\n");
        return VK_ERROR_INITIALIZATION_FAILED;
    }
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
    
    printf("[webvulkan] Creating device...\n");
    result = vkCreateDevice(g_physical_device, &device_info, NULL, &g_device);
    if (result != VK_SUCCESS) {
        printf("[webvulkan] vkCreateDevice failed: %d\n", result);
        return result;
    }
    printf("[webvulkan] Device created\n");
    
    vkGetDeviceQueue(g_device, 0, 0, &g_queue);
    printf("[webvulkan] Queue acquired\n");
    
    VkCommandPoolCreateInfo pool_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = 0,
    };
    
    result = vkCreateCommandPool(g_device, &pool_info, NULL, &g_command_pool);
    if (result != VK_SUCCESS) {
        printf("[webvulkan] vkCreateCommandPool failed: %d\n", result);
        return result;
    }
    printf("[webvulkan] Command pool created\n");
    
    VkCommandBufferAllocateInfo cmd_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = g_command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };
    
    result = vkAllocateCommandBuffers(g_device, &cmd_info, &g_command_buffer);
    if (result != VK_SUCCESS) {
        printf("[webvulkan] vkAllocateCommandBuffers failed: %d\n", result);
        return result;
    }
    printf("[webvulkan] Command buffer allocated\n");
    
    VkBufferCreateInfo buffer_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = sizeof(g_vertices),
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };
    
    result = vkCreateBuffer(g_device, &buffer_info, NULL, &g_vertex_buffer);
    if (result != VK_SUCCESS) {
        printf("[webvulkan] vkCreateBuffer failed: %d\n", result);
        return result;
    }
    printf("[webvulkan] Vertex buffer created (%zu bytes)\n", sizeof(g_vertices));
    
    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(g_device, g_vertex_buffer, &mem_reqs);
    
    VkMemoryAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = mem_reqs.size,
        .memoryTypeIndex = 0,
    };
    
    result = vkAllocateMemory(g_device, &alloc_info, NULL, &g_vertex_memory);
    if (result != VK_SUCCESS) {
        printf("[webvulkan] vkAllocateMemory failed: %d\n", result);
        return result;
    }
    
    void* mapped = NULL;
    result = vkMapMemory(g_device, g_vertex_memory, 0, sizeof(g_vertices), 0, &mapped);
    if (result == VK_SUCCESS && mapped) {
        memcpy(mapped, g_vertices, sizeof(g_vertices));
        vkUnmapMemory(g_device, g_vertex_memory);
    }
    
    vkBindBufferMemory(g_device, g_vertex_buffer, g_vertex_memory, 0);
    printf("[webvulkan] Vertex memory bound\n");
    
    printf("[webvulkan] Vulkan initialization complete\n");
    return VK_SUCCESS;
}

#ifdef __EMSCRIPTEN__

static void init_webgpu_surface(void) {
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
    };
    
    g_surface = wgpuInstanceCreateSurface(instance, &surface_desc);
    
    WGPUSurfaceCapabilities caps;
    wgpuSurfaceGetCapabilities(g_surface, NULL, &caps);
    g_surface_format = caps.formats[0];
    for (uint32_t i = 0; i < caps.formatCount; i++) {
        if (caps.formats[i] == WGPUTextureFormat_BGRA8Unorm ||
            caps.formats[i] == WGPUTextureFormat_RGBA8Unorm) {
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
        surface_texture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessSuboptimal) {
        return;
    }
    
    WGPUTextureViewDescriptor view_desc = {
        .format = g_surface_format,
        .dimension = WGPUTextureViewDimension_2D,
        .aspect = WGPUTextureAspect_All,
        .baseMipLevel = 0,
        .mipLevelCount = 1,
        .baseArrayLayer = 0,
        .arrayLayerCount = 1,
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
    
    WGPURenderPassDescriptor render_pass = {
        .colorAttachmentCount = 1,
        .colorAttachments = &color_attachment,
    };
    
    WGPURenderPassEncoder pass = wgpuCommandEncoderBeginRenderPass(encoder, &render_pass);
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
        if (g_vertex_buffer) vkDestroyBuffer(g_device, g_vertex_buffer, NULL);
        if (g_vertex_memory) vkFreeMemory(g_device, g_vertex_memory, NULL);
        if (g_command_pool) vkDestroyCommandPool(g_device, g_command_pool, NULL);
        vkDestroyDevice(g_device, NULL);
    }
    if (g_instance) {
        vkDestroyInstance(g_instance, NULL);
    }
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
