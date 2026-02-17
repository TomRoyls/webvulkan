#include "webvulkan_internal.h"

static void destroy_device(void* obj) {
    VkDevice device = (VkDevice)obj;
    if (device->wgpu_device) {
        wgpuDeviceRelease(device->wgpu_device);
    }
    if (device->wgpu_queue) {
        wgpuQueueRelease(device->wgpu_queue);
    }
    free(device);
}

static void on_device_lost(WGPUDeviceLostReason reason, char const* message, void* userdata) {
    (void)reason;
    (void)userdata;
    if (message) {
    }
}

static void on_device_error(WGPUErrorType type, char const* message, void* userdata) {
    (void)type;
    (void)userdata;
    if (message) {
    }
}

VkResult vkCreateDevice(
    VkPhysicalDevice physicalDevice,
    const VkDeviceCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDevice* pDevice)
{
    (void)pAllocator;
    
    if (!physicalDevice || !pCreateInfo || !pDevice) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    VkDevice device = wgvk_alloc(sizeof(struct VkDevice_T));
    if (!device) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    
    wgvk_object_init(&device->base, destroy_device);
    device->physical_device = physicalDevice;
    device->wgpu_device = NULL;
    device->wgpu_queue = NULL;
    device->queue_family_index = 0;
    
    WGPUDeviceDescriptor desc = {};
    desc.deviceLostCallback = on_device_lost;
    desc.uncapturedErrorCallback = on_device_error;
    
    if (physicalDevice->wgpu_adapter) {
        device->wgpu_device = wgpuAdapterCreateDevice(physicalDevice->wgpu_adapter, &desc);
    } else {
        device->wgpu_device = wgpuCreateDevice(&desc);
    }
    
    if (!device->wgpu_device) {
        free(device);
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    device->wgpu_queue = wgpuDeviceGetQueue(device->wgpu_device);
    if (!device->wgpu_queue) {
        wgpuDeviceRelease(device->wgpu_device);
        free(device);
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    *pDevice = device;
    return VK_SUCCESS;
}

void vkDestroyDevice(
    VkDevice device,
    const VkAllocationCallbacks* pAllocator)
{
    (void)pAllocator;
    if (device) {
        wgvk_object_release(&device->base);
    }
}

void vkGetDeviceQueue(
    VkDevice device,
    uint32_t queueFamilyIndex,
    uint32_t queueIndex,
    VkQueue* pQueue)
{
    if (!device || !pQueue) {
        return;
    }
    
    VkQueue queue = wgvk_alloc(sizeof(struct VkQueue_T));
    if (!queue) {
        *pQueue = NULL;
        return;
    }
    
    wgvk_object_init(&queue->base, NULL);
    queue->device = device;
    queue->wgpu_queue = device->wgpu_queue;
    wgpuQueueReference(queue->wgpu_queue);
    queue->queue_family_index = queueFamilyIndex;
    queue->queue_index = queueIndex;
    
    *pQueue = queue;
}
