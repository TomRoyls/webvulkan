#include <vulkan.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const float vertices[] = {
     0.0f, -0.5f,       1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,       0.0f, 1.0f, 0.0f,
     0.5f,  0.5f,       0.0f, 0.0f, 1.0f,
};

static float projection_matrix[16] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
};

static VkInstance instance = VK_NULL_HANDLE;
static VkPhysicalDevice physical_device = VK_NULL_HANDLE;
static VkDevice device = VK_NULL_HANDLE;
static VkQueue queue = VK_NULL_HANDLE;
static VkBuffer vertex_buffer = VK_NULL_HANDLE;
static VkDeviceMemory vertex_memory = VK_NULL_HANDLE;
static VkBuffer uniform_buffer = VK_NULL_HANDLE;
static VkDeviceMemory uniform_memory = VK_NULL_HANDLE;
static VkDescriptorSetLayout descriptor_layout = VK_NULL_HANDLE;
static VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
static VkDescriptorSet descriptor_set = VK_NULL_HANDLE;
static VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
static VkPipeline pipeline = VK_NULL_HANDLE;
static VkCommandPool command_pool = VK_NULL_HANDLE;
static VkCommandBuffer command_buffer = VK_NULL_HANDLE;

static const uint32_t vertex_spirv[] = {
    0x07230203, 0x00010000, 0x00080001, 0x00000005, 0x00000000,
    0x0000000b, 0x00000001, 0x00000002, 0x00000006, 0x6e69616d, 0x00000000,
};

static const uint32_t fragment_spirv[] = {
    0x07230203, 0x00010000, 0x00080001, 0x00000004, 0x00000000,
};

static void cleanup(void) {
    if (device) {
        vkDestroyBuffer(device, vertex_buffer, NULL);
        vkFreeMemory(device, vertex_memory, NULL);
        vkDestroyBuffer(device, uniform_buffer, NULL);
        vkFreeMemory(device, uniform_memory, NULL);
        vkDestroyDescriptorPool(device, descriptor_pool, NULL);
        vkDestroyDescriptorSetLayout(device, descriptor_layout, NULL);
        vkDestroyPipelineLayout(device, pipeline_layout, NULL);
        vkDestroyPipeline(device, pipeline, NULL);
        vkDestroyCommandPool(device, command_pool, NULL);
        vkDestroyDevice(device, NULL);
    }
    if (instance) {
        vkDestroyInstance(instance, NULL);
    }
}

int main(int argc, char** argv) {
    VkResult result;
    
    printf("WebVulkan Triangle Example\n");
    printf("==========================\n\n");
    
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
        .enabledExtensionCount = 0,
        .ppEnabledExtensionNames = NULL,
    };
    
    result = vkCreateInstance(&instance_info, NULL, &instance);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to create instance: %d\n", result);
        cleanup();
        return 1;
    }
    printf("Created Vulkan instance\n");
    
    uint32_t device_count = 1;
    result = vkEnumeratePhysicalDevices(instance, &device_count, &physical_device);
    if (result != VK_SUCCESS || device_count == 0) {
        fprintf(stderr, "No physical devices found\n");
        cleanup();
        return 1;
    }
    printf("Selected physical device\n");
    
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
        .enabledExtensionCount = 0,
    };
    
    result = vkCreateDevice(physical_device, &device_info, NULL, &device);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to create device: %d\n", result);
        cleanup();
        return 1;
    }
    printf("Created logical device\n");
    
    vkGetDeviceQueue(device, 0, 0, &queue);
    printf("Acquired queue\n");
    
    VkBufferCreateInfo vb_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = sizeof(vertices),
        .usage = 0x00000010,
        .sharingMode = 0,
    };
    
    result = vkCreateBuffer(device, &vb_info, NULL, &vertex_buffer);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to create vertex buffer: %d\n", result);
        cleanup();
        return 1;
    }
    printf("Created vertex buffer (%zu bytes)\n", sizeof(vertices));
    
    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(device, vertex_buffer, &mem_reqs);
    
    VkMemoryAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = mem_reqs.size,
        .memoryTypeIndex = 0,
    };
    
    result = vkAllocateMemory(device, &alloc_info, NULL, &vertex_memory);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to allocate vertex memory: %d\n", result);
        cleanup();
        return 1;
    }
    
    void* mapped;
    result = vkMapMemory(device, vertex_memory, 0, sizeof(vertices), 0, &mapped);
    if (result == VK_SUCCESS) {
        memcpy(mapped, vertices, sizeof(vertices));
        vkUnmapMemory(device, vertex_memory);
    }
    
    vkBindBufferMemory(device, vertex_buffer, vertex_memory, 0);
    printf("Allocated and filled vertex memory\n");
    
    VkCommandPoolCreateInfo pool_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = 0,
    };
    
    result = vkCreateCommandPool(device, &pool_info, NULL, &command_pool);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to create command pool: %d\n", result);
        cleanup();
        return 1;
    }
    printf("Created command pool\n");
    
    VkCommandBufferAllocateInfo cmd_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = command_pool,
        .level = 0,
        .commandBufferCount = 1,
    };
    
    result = vkAllocateCommandBuffers(device, &cmd_info, &command_buffer);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to allocate command buffer: %d\n", result);
        cleanup();
        return 1;
    }
    printf("Allocated command buffer\n");
    
    printf("\nTriangle initialization complete!\n");
    
    cleanup();
    return 0;
}
