#include "webvulkan_internal.h"

static void destroy_sampler(void *obj) {
	VkSampler sampler = (VkSampler)obj;
	if (sampler->wgpu_sampler) {
		wgpuSamplerRelease(sampler->wgpu_sampler);
	}
	free(sampler);
}

VkResult vkCreateSampler(VkDevice device, const void *pCreateInfo,
                         const VkAllocationCallbacks *pAllocator, VkSampler *pSampler) {
	(void)pCreateInfo;
	(void)pAllocator;

	if (!device || !pSampler) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	VkSampler sampler = wgvk_alloc(sizeof(struct VkSampler_T));
	if (!sampler) {
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	wgvk_object_init(&sampler->base, destroy_sampler);
	sampler->device = device;
	sampler->wgpu_sampler = NULL;

	WGPUSamplerDescriptor desc = {
	    .addressModeU = WGPUAddressMode_ClampToEdge,
	    .addressModeV = WGPUAddressMode_ClampToEdge,
	    .addressModeW = WGPUAddressMode_ClampToEdge,
	    .magFilter = WGPUFilterMode_Linear,
	    .minFilter = WGPUFilterMode_Linear,
	    .mipmapFilter = WGPUMipmapFilterMode_Linear,
	    .lodMinClamp = 0.0f,
	    .lodMaxClamp = 32.0f,
	    .compare = WGPUCompareFunction_Undefined,
	    .maxAnisotropy = 1,
	};

	sampler->wgpu_sampler = wgpuDeviceCreateSampler(device->wgpu_device, &desc);

	if (!sampler->wgpu_sampler) {
		free(sampler);
		return VK_ERROR_OUT_OF_DEVICE_MEMORY;
	}

	*pSampler = sampler;
	return VK_SUCCESS;
}

void vkDestroySampler(VkDevice device, VkSampler sampler, const VkAllocationCallbacks *pAllocator) {
	(void)device;
	(void)pAllocator;
	if (sampler) {
		wgvk_object_release(&sampler->base);
	}
}
