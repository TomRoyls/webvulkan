#include "vulkan.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    printf("Shadow Mapping Example\n");
    printf("Demonstrates depth-only render pass for shadow maps\n");
    
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
    
    VkImageCreateInfo shadow_info = {0};
    shadow_info.sType = 14;
    shadow_info.imageType = 2;
    shadow_info.format = 124;
    shadow_info.extent[0] = 2048;
    shadow_info.extent[1] = 2048;
    shadow_info.extent[2] = 1;
    shadow_info.mipLevels = 1;
    shadow_info.arrayLayers = 1;
    shadow_info.samples = 1;
    shadow_info.tiling = 1;
    shadow_info.usage = 0x00000010 | 0x00000100;
    
    VkImage shadow_map = NULL;
    vkCreateImage(device, &shadow_info, NULL, &shadow_map);
    
    if (shadow_map) {
        printf("Created shadow map depth texture (2048x2048)\n");
    }
    
    VkImageViewCreateInfo view_info = {0};
    view_info.sType = 15;
    view_info.image = (VkImage)shadow_map;
    view_info.viewType = 1;
    view_info.format = 124;
    
    VkImageView shadow_view = NULL;
    vkCreateImageView(device, &view_info, NULL, &shadow_view);
    
    if (shadow_view) {
        printf("Created shadow map depth view\n");
    }
    
    VkRenderPassCreateInfo rp_info = {0};
    rp_info.sType = 38;
    rp_info.attachmentCount = 1;
    rp_info.subpassCount = 1;
    
    VkRenderPass shadow_pass = NULL;
    VkRenderPass scene_pass = NULL;
    
    vkCreateRenderPass(device, &rp_info, NULL, &shadow_pass);
    vkCreateRenderPass(device, &rp_info, NULL, &scene_pass);
    
    if (shadow_pass && scene_pass) {
        printf("Created shadow and scene render passes\n");
    }
    
    VkPipelineLayoutCreateInfo layout_info = {0};
    layout_info.sType = 30;
    
    VkPipelineLayout shadow_layout = NULL;
    VkPipelineLayout scene_layout = NULL;
    
    vkCreatePipelineLayout(device, &layout_info, NULL, &shadow_layout);
    vkCreatePipelineLayout(device, &layout_info, NULL, &scene_layout);
    
    VkSamplerCreateInfo sampler_info = {0};
    sampler_info.sType = 31;
    sampler_info.magFilter = 0;
    sampler_info.minFilter = 0;
    sampler_info.addressModeU = 0;
    sampler_info.addressModeV = 0;
    sampler_info.addressModeW = 0;
    sampler_info.compareEnable = 1;
    sampler_info.compareOp = 1;
    
    VkSampler shadow_sampler = NULL;
    vkCreateSampler(device, &sampler_info, NULL, &shadow_sampler);
    
    if (shadow_sampler) {
        printf("Created shadow comparison sampler\n");
    }
    
    VkShaderModuleCreateInfo shader_info = {0};
    shader_info.sType = 16;
    
    VkShaderModule shadow_vert = NULL;
    VkShaderModule scene_vert = NULL;
    VkShaderModule scene_frag = NULL;
    
    vkCreateShaderModule(device, &shader_info, NULL, &shadow_vert);
    vkCreateShaderModule(device, &shader_info, NULL, &scene_vert);
    vkCreateShaderModule(device, &shader_info, NULL, &scene_frag);
    
    if (shadow_vert && scene_vert && scene_frag) {
        printf("Created shadow mapping shader modules\n");
    }
    
    printf("Shadow mapping example completed\n");
    printf("Note: Full shadow mapping requires render pass synchronization\n");
    
    vkDestroyShaderModule(device, scene_frag, NULL);
    vkDestroyShaderModule(device, scene_vert, NULL);
    vkDestroyShaderModule(device, shadow_vert, NULL);
    vkDestroySampler(device, shadow_sampler, NULL);
    vkDestroyPipelineLayout(device, scene_layout, NULL);
    vkDestroyPipelineLayout(device, shadow_layout, NULL);
    vkDestroyRenderPass(device, scene_pass, NULL);
    vkDestroyRenderPass(device, shadow_pass, NULL);
    vkDestroyImageView(device, shadow_view, NULL);
    vkDestroyImage(device, shadow_map, NULL);
    vkDestroyDevice(device, NULL);
    vkDestroyInstance(instance, NULL);
    
    return 0;
}
