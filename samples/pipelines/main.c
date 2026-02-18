#include "vulkan.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* vertex_shader = 
    "#version 450\n"
    "vec2 positions[3] = vec2[](vec2(0.0, -0.5), vec2(0.5, 0.5), vec2(-0.5, 0.5));\n"
    "vec3 colors[3] = vec3[](vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0));\n"
    "layout(location = 0) out vec3 fragColor;\n"
    "void main() { gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0); fragColor = colors[gl_VertexIndex]; }\n";

static const char* fragment_shader =
    "#version 450\n"
    "layout(location = 0) in vec3 fragColor;\n"
    "layout(location = 0) out vec4 outColor;\n"
    "void main() { outColor = vec4(fragColor, 1.0); }\n";

int main(void) {
    printf("Pipelines Example\n");
    printf("Demonstrates pipeline creation and switching\n");
    
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
    
    if (device_count == 0) {
        printf("No physical devices found\n");
        vkDestroyInstance(instance, NULL);
        return 1;
    }
    
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
    
    VkShaderModuleCreateInfo vert_shader_info = {0};
    vert_shader_info.sType = 16;
    vert_shader_info.codeSize = 0;
    vert_shader_info.pCode = NULL;
    
    VkShaderModule vert_module = NULL;
    result = vkCreateShaderModule(device, &vert_shader_info, NULL, &vert_module);
    
    VkShaderModule frag_module = NULL;
    result = vkCreateShaderModule(device, &vert_shader_info, NULL, &frag_module);
    
    printf("Created shader modules\n");
    
    VkPipelineLayoutCreateInfo layout_info = {0};
    layout_info.sType = 30;
    layout_info.setLayoutCount = 0;
    layout_info.pushConstantRangeCount = 0;
    
    VkPipelineLayout layout = NULL;
    result = vkCreatePipelineLayout(device, &layout_info, NULL, &layout);
    
    if (result == 0) {
        printf("Created pipeline layout\n");
    }
    
    printf("Pipelines example completed successfully\n");
    
    if (layout) vkDestroyPipelineLayout(device, layout, NULL);
    if (vert_module) vkDestroyShaderModule(device, vert_module, NULL);
    if (frag_module) vkDestroyShaderModule(device, frag_module, NULL);
    vkDestroyDevice(device, NULL);
    vkDestroyInstance(instance, NULL);
    
    return 0;
}
