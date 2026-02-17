#include "webvulkan_internal.h"
#include <stdio.h>

static void destroy_instance(void* obj) {
    VkInstance instance = (VkInstance)obj;
    if (instance->wgpu_instance) {
        wgpuInstanceRelease(instance->wgpu_instance);
    }
    free(instance->application_name);
    free(instance->engine_name);
    free(instance);
}

VkResult vkCreateInstance(
    const VkInstanceCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkInstance* pInstance)
{
    (void)pAllocator;
    
    if (!pCreateInfo || !pInstance) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    VkInstance instance = wgvk_alloc(sizeof(struct VkInstance_T));
    if (!instance) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    
    wgvk_object_init(&instance->base, destroy_instance);
    
    instance->api_version = VK_API_VERSION_1_4;
    instance->application_name = NULL;
    instance->engine_name = NULL;
    
    if (pCreateInfo->pApplicationInfo) {
        const VkApplicationInfo* app_info = pCreateInfo->pApplicationInfo;
        instance->api_version = app_info->apiVersion;
        if (app_info->pApplicationName) {
            instance->application_name = wgvk_strdup(app_info->pApplicationName);
        }
        if (app_info->pEngineName) {
            instance->engine_name = wgvk_strdup(app_info->pEngineName);
        }
    }
    
    WGPUInstanceDescriptor desc = {};
    instance->wgpu_instance = wgpuCreateInstance(&desc);
    
    if (!instance->wgpu_instance) {
        free(instance);
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    *pInstance = instance;
    return VK_SUCCESS;
}

void vkDestroyInstance(
    VkInstance instance,
    const VkAllocationCallbacks* pAllocator)
{
    (void)pAllocator;
    if (instance) {
        wgvk_object_release(&instance->base);
    }
}

VkResult vkEnumeratePhysicalDevices(
    VkInstance instance,
    uint32_t* pPhysicalDeviceCount,
    VkPhysicalDevice* pPhysicalDevices)
{
    if (!instance || !pPhysicalDeviceCount) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    if (!pPhysicalDevices) {
        *pPhysicalDeviceCount = 1;
        return VK_SUCCESS;
    }
    
    if (*pPhysicalDeviceCount < 1) {
        return VK_INCOMPLETE;
    }
    
    VkPhysicalDevice phys_dev = wgvk_alloc(sizeof(struct VkPhysicalDevice_T));
    if (!phys_dev) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    
    wgvk_object_init(&phys_dev->base, NULL);
    phys_dev->instance = instance;
    phys_dev->wgpu_adapter = NULL;
    phys_dev->api_version = instance->api_version;
    
    pPhysicalDevices[0] = phys_dev;
    *pPhysicalDeviceCount = 1;
    
    return VK_SUCCESS;
}
