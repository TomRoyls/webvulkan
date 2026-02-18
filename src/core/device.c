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

typedef struct {
    VkDevice device;
    bool completed;
    WGPUDevice wgpu_device;
} DeviceRequestContext;

static void on_device_request_done(WGPURequestDeviceStatus status, WGPUDevice device, WGPUStringView message, void* userdata1, void* userdata2) {
    (void)message;
    (void)userdata2;
    DeviceRequestContext* ctx = (DeviceRequestContext*)userdata1;
    if (status == WGPURequestDeviceStatus_Success) {
        ctx->wgpu_device = device;
    }
    ctx->completed = true;
}

VkResult vkCreateDevice(
    VkPhysicalDevice physicalDevice,
    const VkDeviceCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDevice* pDevice)
{
    (void)pAllocator;
    (void)pCreateInfo;
    
    if (!physicalDevice || !pDevice) {
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
    device->push_constant_buffer = NULL;
    
    DeviceRequestContext ctx = { .device = device, .completed = false, .wgpu_device = NULL };
    
    WGPUDeviceDescriptor desc = WGPU_DEVICE_DESCRIPTOR_INIT;
    
    WGPURequestDeviceCallbackInfo callbackInfo = {
        .nextInChain = NULL,
        .mode = WGPUCallbackMode_AllowProcessEvents,
        .callback = on_device_request_done,
        .userdata1 = &ctx,
        .userdata2 = NULL,
    };
    
    if (physicalDevice->wgpu_adapter) {
        wgpuAdapterRequestDevice(physicalDevice->wgpu_adapter, &desc, callbackInfo);
    }
    
    if (!ctx.completed || !ctx.wgpu_device) {
        free(device);
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    device->wgpu_device = ctx.wgpu_device;
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
    wgpuQueueAddRef(queue->wgpu_queue);
    queue->queue_family_index = queueFamilyIndex;
    queue->queue_index = queueIndex;
    
    *pQueue = queue;
}
