#include <stdio.h>
#include "webvulkan_internal.h"

static void destroy_physical_device(void *obj) {
	VkPhysicalDevice phys_dev = (VkPhysicalDevice)obj;
#ifndef __EMSCRIPTEN__
	if (phys_dev->wgpu_adapter) {
		wgpuAdapterRelease(phys_dev->wgpu_adapter);
	}
#endif
	wgvk_free(phys_dev);
}

static void destroy_instance(void *obj) {
	VkInstance instance = (VkInstance)obj;
	if (instance->phys_dev) {
		wgvk_object_release(&instance->phys_dev->base);
	}
	if (instance->wgpu_instance) {
		wgpuInstanceRelease(instance->wgpu_instance);
	}
	wgvk_free(instance->application_name);
	wgvk_free(instance->engine_name);
	wgvk_free(instance);
}

VkResult vkCreateInstance(const VkInstanceCreateInfo *pCreateInfo,
                          const VkAllocationCallbacks *pAllocator, VkInstance *pInstance) {
	(void)pAllocator;

	if (!pCreateInfo || !pInstance) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	VkInstance instance = wgvk_alloc(sizeof(struct VkInstance_T));
	if (!instance) {
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	wgvk_object_init(&instance->base, destroy_instance);

	instance->api_version = VK_API_VERSION_1_0;
	instance->application_name = NULL;
	instance->engine_name = NULL;
	instance->phys_dev = NULL;

	if (pCreateInfo->pApplicationInfo) {
		const VkApplicationInfo *app_info = pCreateInfo->pApplicationInfo;
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
		wgvk_free(instance);
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	*pInstance = instance;
	return VK_SUCCESS;
}

void vkDestroyInstance(VkInstance instance, const VkAllocationCallbacks *pAllocator) {
	(void)pAllocator;
	if (instance) {
		wgvk_object_release(&instance->base);
	}
}

VkResult vkEnumeratePhysicalDevices(VkInstance instance, uint32_t *pPhysicalDeviceCount,
                                    VkPhysicalDevice *pPhysicalDevices) {
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

	/* Return the cached physical device if already created.  Repeated calls
	 * to vkEnumeratePhysicalDevices MUST return the same handle. */
	if (!instance->phys_dev) {
		VkPhysicalDevice phys_dev = wgvk_alloc(sizeof(struct VkPhysicalDevice_T));
		if (!phys_dev) {
			return VK_ERROR_OUT_OF_HOST_MEMORY;
		}
		wgvk_object_init(&phys_dev->base, destroy_physical_device);
		phys_dev->instance = instance;
		phys_dev->wgpu_adapter = NULL;
		phys_dev->api_version = instance->api_version;
		instance->phys_dev = phys_dev;
	}

	/* Retain so the caller's copy keeps the object alive independently. */
	wgvk_object_retain(&instance->phys_dev->base);
	pPhysicalDevices[0] = instance->phys_dev;
	*pPhysicalDeviceCount = 1;

	return VK_SUCCESS;
}
