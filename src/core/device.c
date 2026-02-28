#include "webvulkan_internal.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

static void destroy_device(void *obj) {
	VkDevice device = (VkDevice)obj;
#ifndef __EMSCRIPTEN__
	if (device->wgpu_device) {
		wgpuDeviceRelease(device->wgpu_device);
	}
#endif
	if (device->wgpu_queue) {
		wgpuQueueRelease(device->wgpu_queue);
	}
	if (device->push_constant_buffer) {
		wgpuBufferRelease(device->push_constant_buffer);
	}
	wgvk_free(device);
}

VkResult vkCreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo *pCreateInfo,
                        const VkAllocationCallbacks *pAllocator, VkDevice *pDevice) {
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

#ifdef __EMSCRIPTEN__
	device->wgpu_device = emscripten_webgpu_get_device();
	if (!device->wgpu_device) {
		wgvk_free(device);
		return VK_ERROR_INITIALIZATION_FAILED;
	}
#else
	if (physicalDevice->wgpu_adapter) {
		WGPUDeviceDescriptor desc = WGPU_DEVICE_DESCRIPTOR_INIT;
		device->wgpu_device = wgpuAdapterRequestDeviceSync(physicalDevice->wgpu_adapter, &desc);
	}
	if (!device->wgpu_device) {
		wgvk_free(device);
		return VK_ERROR_INITIALIZATION_FAILED;
	}
#endif

	device->wgpu_queue = wgpuDeviceGetQueue(device->wgpu_device);

	if (!device->wgpu_queue) {
#ifndef __EMSCRIPTEN__
		wgpuDeviceRelease(device->wgpu_device);
#endif
		wgvk_free(device);
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	*pDevice = device;
	return VK_SUCCESS;
}

void vkDestroyDevice(VkDevice device, const VkAllocationCallbacks *pAllocator) {
	(void)pAllocator;
	if (device) {
		wgvk_object_release(&device->base);
	}
}

static void destroy_queue(void *obj) {
	VkQueue queue = (VkQueue)obj;
	if (queue->wgpu_queue) {
		wgpuQueueRelease(queue->wgpu_queue);
	}
	wgvk_free(queue);
}

void vkGetDeviceQueue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex,
                      VkQueue *pQueue) {
	if (!device || !pQueue) {
		return;
	}

	VkQueue queue = wgvk_alloc(sizeof(struct VkQueue_T));
	if (!queue) {
		*pQueue = NULL;
		return;
	}

	wgvk_object_init(&queue->base, destroy_queue);
	queue->device = device;
	queue->wgpu_queue = device->wgpu_queue;
	wgpuQueueAddRef(queue->wgpu_queue);
	queue->queue_family_index = queueFamilyIndex;
	queue->queue_index = queueIndex;

	*pQueue = queue;
}

VkResult vkDeviceWaitIdle(VkDevice device) {
	(void)device;
	/* WebGPU does not expose a synchronous CPU-GPU fence.  Work submission is
	 * fully asynchronous; callers that need ordering must use semaphores or
	 * fences provided by the swap-chain / present layer.  Returning success
	 * here is the correct no-op for a WebGPU back-end. */
	return VK_SUCCESS;
}
