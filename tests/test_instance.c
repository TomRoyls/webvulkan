#include "vulkan.h"
#include <stdio.h>
#include <assert.h>

int main(void) {
    VkInstanceCreateInfo create_info = {0};
    create_info.sType = 1;
    
    VkInstance instance = NULL;
    VkResult result = vkCreateInstance(&create_info, NULL, &instance);
    
    if (result == VK_SUCCESS && instance != NULL) {
        printf("test_instance: PASSED\n");
        
        uint32_t count = 0;
        vkEnumeratePhysicalDevices(instance, &count, NULL);
        printf("  Found %u physical devices\n", count);
        
        vkDestroyInstance(instance, NULL);
        return 0;
    }
    
    printf("test_instance: FAILED (result=%d)\n", result);
    return 1;
}
