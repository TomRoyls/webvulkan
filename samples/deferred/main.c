#include "vulkan.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    printf("Deferred Rendering Example\n");
    printf("Demonstrates G-Buffer and multi-pass deferred shading\n");
    
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
    
    VkImageCreateInfo gbuffer_info = {0};
    gbuffer_info.sType = 14;
    gbuffer_info.imageType = 2;
    gbuffer_info.format = 44;
    gbuffer_info.extent[0] = 800;
    gbuffer_info.extent[1] = 600;
    gbuffer_info.extent[2] = 1;
    gbuffer_info.mipLevels = 1;
    gbuffer_info.arrayLayers = 1;
    gbuffer_info.samples = 1;
    gbuffer_info.tiling = 1;
    gbuffer_info.usage = 0x00000004 | 0x00000010;
    
    VkImage position_buffer = NULL;
    VkImage normal_buffer = NULL;
    VkImage albedo_buffer = NULL;
    
    vkCreateImage(device, &gbuffer_info, NULL, &position_buffer);
    vkCreateImage(device, &gbuffer_info, NULL, &normal_buffer);
    vkCreateImage(device, &gbuffer_info, NULL, &albedo_buffer);
    
    if (position_buffer && normal_buffer && albedo_buffer) {
        printf("Created G-Buffer attachments (position, normal, albedo)\n");
    }
    
    VkRenderPassCreateInfo rp_info = {0};
    rp_info.sType = 38;
    rp_info.attachmentCount = 4;
    rp_info.subpassCount = 1;
    
    VkRenderPass render_pass = NULL;
    vkCreateRenderPass(device, &rp_info, NULL, &render_pass);
    
    if (render_pass) {
        printf("Created deferred render pass\n");
    }
    
    VkPipelineLayoutCreateInfo layout_info = {0};
    layout_info.sType = 30;
    
    VkPipelineLayout geometry_layout = NULL;
    VkPipelineLayout lighting_layout = NULL;
    vkCreatePipelineLayout(device, &layout_info, NULL, &geometry_layout);
    vkCreatePipelineLayout(device, &layout_info, NULL, &lighting_layout);
    
    if (geometry_layout && lighting_layout) {
        printf("Created geometry and lighting pipeline layouts\n");
    }
    
    VkShaderModuleCreateInfo shader_info = {0};
    shader_info.sType = 16;
    
    VkShaderModule geometry_vert = NULL;
    VkShaderModule geometry_frag = NULL;
    VkShaderModule lighting_vert = NULL;
    VkShaderModule lighting_frag = NULL;
    
    vkCreateShaderModule(device, &shader_info, NULL, &geometry_vert);
    vkCreateShaderModule(device, &shader_info, NULL, &geometry_frag);
    vkCreateShaderModule(device, &shader_info, NULL, &lighting_vert);
    vkCreateShaderModule(device, &shader_info, NULL, &lighting_frag);
    
    if (geometry_vert && geometry_frag && lighting_vert && lighting_frag) {
        printf("Created geometry and lighting shader modules\n");
    }
    
    printf("Deferred rendering example completed\n");
    printf("Note: Full deferred rendering requires subpass dependencies\n");
    
    vkDestroyShaderModule(device, lighting_frag, NULL);
    vkDestroyShaderModule(device, lighting_vert, NULL);
    vkDestroyShaderModule(device, geometry_frag, NULL);
    vkDestroyShaderModule(device, geometry_vert, NULL);
    vkDestroyPipelineLayout(device, lighting_layout, NULL);
    vkDestroyPipelineLayout(device, geometry_layout, NULL);
    vkDestroyRenderPass(device, render_pass, NULL);
    vkDestroyImage(device, albedo_buffer, NULL);
    vkDestroyImage(device, normal_buffer, NULL);
    vkDestroyImage(device, position_buffer, NULL);
    vkDestroyDevice(device, NULL);
    vkDestroyInstance(instance, NULL);
    
    return 0;
}
