#include "vulkan.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    printf("Push Constants Example\n");
    printf("Demonstrates push constant usage via emulated uniform buffers\n");
    
    VkInstanceCreateInfo instance_info = {0};
    instance_info.sType = 1;
    
    VkInstance instance = NULL;
    VkResult result = vkCreateInstance(&instance_info, NULL, &instance);
    
    if (result != 0) {
        printf("Failed to create instance: %d\n", result);
        return 1;
    }
    
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, NULL);
    
    VkPhysicalDevice physical_device = NULL;
    vkEnumeratePhysicalDevices(instance, &device_count, &physical_device);
    
    float queue_priority = 1.0f;
    VkDeviceQueueCreateInfo queue_info = {0};
    queue_info.sType = 2;
    queue_info.queueFamilyIndex = 0;
    queue_info.queueCount = 1;
    queue_info.pQueuePriorities = &queue_priority;
    
    VkDeviceCreateInfo device_info = {0};
    device_info.sType = 3;
    device_info.queueCreateInfoCount = 1;
    device_info.pQueueCreateInfos = &queue_info;
    
    VkDevice device = NULL;
    result = vkCreateDevice(physical_device, &device_info, NULL, &device);
    
    if (result != 0) {
        printf("Failed to create device: %d\n", result);
        vkDestroyInstance(instance, NULL);
        return 1;
    }
    
    typedef struct {
        float mvp[16];
    } PushConstants;
    
    PushConstants pc;
    for (int i = 0; i < 16; i++) pc.mvp[i] = (i % 5 == 0) ? 1.0f : 0.0f;
    
    typedef struct {
        VkStructureType sType;
        const void* pNext;
        uint32_t stageFlags;
        uint32_t offset;
        uint32_t size;
    } VkPushConstantRange;
    
    VkPushConstantRange push_range = {0};
    push_range.stageFlags = 0x00000001 | 0x00000002;
    push_range.offset = 0;
    push_range.size = sizeof(PushConstants);
    
    VkPipelineLayoutCreateInfo layout_info = {0};
    layout_info.sType = 30;
    layout_info.setLayoutCount = 0;
    layout_info.pushConstantRangeCount = 1;
    layout_info.pPushConstantRanges = &push_range;
    
    VkPipelineLayout layout = NULL;
    result = vkCreatePipelineLayout(device, &layout_info, NULL, &layout);
    
    if (result == 0) {
        printf("Created pipeline layout with push constants\n");
    }
    
    VkCommandPoolCreateInfo pool_info = {0};
    pool_info.sType = 39;
    pool_info.queueFamilyIndex = 0;
    
    VkCommandPool cmd_pool = NULL;
    result = vkCreateCommandPool(device, &pool_info, NULL, &cmd_pool);
    
    if (result == 0) {
        printf("Created command pool\n");
    }
    
    VkCommandBufferAllocateInfo alloc_info = {0};
    alloc_info.sType = 40;
    alloc_info.commandPool = cmd_pool;
    alloc_info.level = 0;
    alloc_info.commandBufferCount = 1;
    
    VkCommandBuffer cmd_buffer = NULL;
    result = vkAllocateCommandBuffers(device, &alloc_info, &cmd_buffer);
    
    if (result == 0) {
        printf("Allocated command buffer\n");
        
        VkCommandBufferBeginInfo begin_info = {0};
        begin_info.sType = 41;
        
        vkBeginCommandBuffer(cmd_buffer, &begin_info);
        
        vkCmdPushConstants(cmd_buffer, layout, 0x00000001 | 0x00000002, 
                          0, sizeof(PushConstants), &pc);
        
        printf("Pushed constants to command buffer\n");
        
        vkEndCommandBuffer(cmd_buffer);
        printf("Ended command buffer\n");
    }
    
    printf("Push constants example completed successfully\n");
    
    if (cmd_buffer) vkFreeCommandBuffers(device, cmd_pool, 1, &cmd_buffer);
    if (cmd_pool) vkDestroyCommandPool(device, cmd_pool, NULL);
    if (layout) vkDestroyPipelineLayout(device, layout, NULL);
    vkDestroyDevice(device, NULL);
    vkDestroyInstance(instance, NULL);
    
    return 0;
}
