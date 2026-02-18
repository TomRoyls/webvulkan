#include "vulkan.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    printf("Compute Shader Example\n");
    printf("Demonstrates compute pipeline and dispatch\n");
    
    VkInstanceCreateInfo instance_info = {0};
    instance_info.sType = 1;
    
    VkInstance instance = NULL;
    VkResult result = vkCreateInstance(&instance_info, NULL, &instance);
    if (result != 0) { printf("Failed to create instance\n"); return 1; }
    
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
    if (result != 0) { vkDestroyInstance(instance, NULL); return 1; }
    
    VkBufferCreateInfo buffer_info = {0};
    buffer_info.sType = 12;
    buffer_info.size = 1024;
    buffer_info.usage = 0x00000020;
    
    VkBuffer storage_buffer = NULL;
    result = vkCreateBuffer(device, &buffer_info, NULL, &storage_buffer);
    
    if (result == 0) {
        printf("Created storage buffer for compute\n");
    }
    
    VkShaderModuleCreateInfo shader_info = {0};
    shader_info.sType = 16;
    shader_info.codeSize = 0;
    shader_info.pCode = NULL;
    
    VkShaderModule compute_shader = NULL;
    vkCreateShaderModule(device, &shader_info, NULL, &compute_shader);
    
    VkPipelineLayoutCreateInfo layout_info = {0};
    layout_info.sType = 30;
    
    VkPipelineLayout layout = NULL;
    vkCreatePipelineLayout(device, &layout_info, NULL, &layout);
    
    VkPipelineShaderStageCreateInfo stage_info = {0};
    stage_info.sType = 18;
    stage_info.stage = 0x00000020;
    stage_info.module = compute_shader;
    stage_info.pName = "main";
    
    VkComputePipelineCreateInfo pipeline_info = {0};
    pipeline_info.sType = 29;
    pipeline_info.stage = stage_info;
    pipeline_info.layout = layout;
    
    VkPipeline compute_pipeline = NULL;
    result = vkCreateComputePipelines(device, NULL, 1, &pipeline_info, NULL, &compute_pipeline);
    
    if (result == 0) {
        printf("Created compute pipeline\n");
    }
    
    VkCommandPoolCreateInfo pool_info = {0};
    pool_info.sType = 39;
    pool_info.queueFamilyIndex = 0;
    
    VkCommandPool cmd_pool = NULL;
    vkCreateCommandPool(device, &pool_info, NULL, &cmd_pool);
    
    VkCommandBufferAllocateInfo alloc_info = {0};
    alloc_info.sType = 40;
    alloc_info.commandPool = cmd_pool;
    alloc_info.level = 0;
    alloc_info.commandBufferCount = 1;
    
    VkCommandBuffer cmd_buffer = NULL;
    vkAllocateCommandBuffers(device, &alloc_info, &cmd_buffer);
    
    if (cmd_buffer) {
        VkCommandBufferBeginInfo begin_info = {0};
        begin_info.sType = 41;
        
        vkBeginCommandBuffer(cmd_buffer, &begin_info);
        
        vkCmdBindPipeline(cmd_buffer, 0x00000001, compute_pipeline);
        
        vkCmdDispatch(cmd_buffer, 1, 1, 1);
        printf("Dispatched compute workgroups\n");
        
        vkEndCommandBuffer(cmd_buffer);
    }
    
    printf("Compute shader example completed\n");
    
    vkFreeCommandBuffers(device, cmd_pool, 1, &cmd_buffer);
    vkDestroyCommandPool(device, cmd_pool, NULL);
    vkDestroyPipeline(device, compute_pipeline, NULL);
    vkDestroyPipelineLayout(device, layout, NULL);
    vkDestroyShaderModule(device, compute_shader, NULL);
    vkDestroyBuffer(device, storage_buffer, NULL);
    vkDestroyDevice(device, NULL);
    vkDestroyInstance(instance, NULL);
    
    return 0;
}
