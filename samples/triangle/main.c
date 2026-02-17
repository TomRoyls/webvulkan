#include <vulkan.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

static VkInstance instance = NULL;
static VkPhysicalDevice physical_device = NULL;
static VkDevice device = NULL;
static VkQueue queue = NULL;
static VkCommandPool command_pool = NULL;
static VkCommandBuffer command_buffer = NULL;
static VkBuffer vertex_buffer = NULL;
static VkShaderModule vert_shader = NULL;
static VkShaderModule frag_shader = NULL;
static VkPipelineLayout pipeline_layout = NULL;
static VkPipeline pipeline = NULL;
static VkRenderPass render_pass = NULL;
static VkFramebuffer framebuffer = NULL;
static VkImage color_image = NULL;
static VkImageView color_image_view = NULL;

typedef struct {
    float pos[2];
    float color[3];
} Vertex;

static const Vertex vertices[] = {
    {{ 0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{ 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
};

static uint32_t read_file(const char* filename, uint32_t** data) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        return 0;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    *data = malloc(size);
    if (!*data) {
        fclose(f);
        return 0;
    }
    
    size_t read = fread(*data, 1, size, f);
    fclose(f);
    
    return (uint32_t)read;
}

static VkResult create_shader_module(VkDevice dev, const char* filename, VkShaderModule* module) {
    uint32_t* code = NULL;
    uint32_t size = read_file(filename, &code);
    if (!code || size == 0) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    VkShaderModuleCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = size,
        .pCode = code,
    };
    
    VkResult result = vkCreateShaderModule(dev, &create_info, NULL, module);
    free(code);
    return result;
}

static int init_vulkan(void) {
    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Triangle",
        .applicationVersion = 0,
        .pEngineName = "webvulkan",
        .engineVersion = 0,
        .apiVersion = VK_API_VERSION_1_4,
    };
    
    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app_info,
    };
    
    VkResult result = vkCreateInstance(&create_info, NULL, &instance);
    if (result != VK_SUCCESS) {
        printf("Failed to create instance: %d\n", result);
        return -1;
    }
    
    uint32_t device_count = 1;
    result = vkEnumeratePhysicalDevices(instance, &device_count, &physical_device);
    if (result != VK_SUCCESS || device_count == 0) {
        printf("Failed to enumerate physical devices: %d\n", result);
        return -1;
    }
    
    float queue_priority = 1.0f;
    VkDeviceQueueCreateInfo queue_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = 0,
        .queueCount = 1,
        .pQueuePriorities = &queue_priority,
    };
    
    VkDeviceCreateInfo device_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queue_create_info,
    };
    
    result = vkCreateDevice(physical_device, &device_create_info, NULL, &device);
    if (result != VK_SUCCESS) {
        printf("Failed to create device: %d\n", result);
        return -1;
    }
    
    vkGetDeviceQueue(device, 0, 0, &queue);
    
    VkCommandPoolCreateInfo pool_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = 0,
    };
    
    result = vkCreateCommandPool(device, &pool_info, NULL, &command_pool);
    if (result != VK_SUCCESS) {
        printf("Failed to create command pool: %d\n", result);
        return -1;
    }
    
    return 0;
}

static int create_resources(void) {
    VkBufferCreateInfo buffer_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = sizeof(vertices),
        .usage = 0x00000001,
    };
    
    VkResult result = vkCreateBuffer(device, &buffer_info, NULL, &vertex_buffer);
    if (result != VK_SUCCESS) {
        printf("Failed to create vertex buffer: %d\n", result);
        return -1;
    }
    
    result = create_shader_module(device, "shaders/triangle.vert.spv", &vert_shader);
    if (result != VK_SUCCESS) {
        printf("Failed to create vertex shader: %d\n", result);
        return -1;
    }
    
    result = create_shader_module(device, "shaders/triangle.frag.spv", &frag_shader);
    if (result != VK_SUCCESS) {
        printf("Failed to create fragment shader: %d\n", result);
        return -1;
    }
    
    VkPipelineLayoutCreateInfo layout_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 0,
    };
    
    result = vkCreatePipelineLayout(device, &layout_info, NULL, &pipeline_layout);
    if (result != VK_SUCCESS) {
        printf("Failed to create pipeline layout: %d\n", result);
        return -1;
    }
    
    return 0;
}

static int render(void) {
    VkCommandBufferAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = command_pool,
        .level = 0,
        .commandBufferCount = 1,
    };
    
    VkResult result = vkAllocateCommandBuffers(device, &alloc_info, &command_buffer);
    if (result != VK_SUCCESS) {
        return -1;
    }
    
    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    };
    
    result = vkBeginCommandBuffer(command_buffer, &begin_info);
    if (result != VK_SUCCESS) {
        return -1;
    }
    
    vkCmdBindPipeline(command_buffer, 0, pipeline);
    vkCmdBindVertexBuffers(command_buffer, 0, 1, &vertex_buffer, (VkDeviceSize[]){0});
    vkCmdDraw(command_buffer, 3, 1, 0, 0);
    
    result = vkEndCommandBuffer(command_buffer);
    if (result != VK_SUCCESS) {
        return -1;
    }
    
    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &command_buffer,
    };
    
    result = vkQueueSubmit(queue, 1, &submit_info, NULL);
    if (result != VK_SUCCESS) {
        return -1;
    }
    
    vkQueueWaitIdle(queue);
    
    return 0;
}

static void cleanup(void) {
    if (pipeline) vkDestroyPipeline(device, pipeline, NULL);
    if (pipeline_layout) vkDestroyPipelineLayout(device, pipeline_layout, NULL);
    if (frag_shader) vkDestroyShaderModule(device, frag_shader, NULL);
    if (vert_shader) vkDestroyShaderModule(device, vert_shader, NULL);
    if (vertex_buffer) vkDestroyBuffer(device, vertex_buffer, NULL);
    if (command_pool) vkDestroyCommandPool(device, command_pool, NULL);
    if (device) vkDestroyDevice(device, NULL);
    if (instance) vkDestroyInstance(instance, NULL);
}

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>

static void main_loop(void) {
    render();
}

int main(void) {
    if (init_vulkan() != 0) {
        return 1;
    }
    
    if (create_resources() != 0) {
        cleanup();
        return 1;
    }
    
    emscripten_set_main_loop(main_loop, 0, 1);
    
    cleanup();
    return 0;
}
#else
int main(void) {
    if (init_vulkan() != 0) {
        return 1;
    }
    
    if (create_resources() != 0) {
        cleanup();
        return 1;
    }
    
    for (int i = 0; i < 100; i++) {
        if (render() != 0) {
            cleanup();
            return 1;
        }
    }
    
    cleanup();
    return 0;
}
#endif
