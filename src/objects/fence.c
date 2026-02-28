#include "webvulkan_internal.h"

static void destroy_fence(void *obj) {
	VkFence fence = (VkFence)obj;
	wgvk_free(fence);
}

VkResult vkCreateFence(VkDevice device, const VkFenceCreateInfo *pCreateInfo,
                       const VkAllocationCallbacks *pAllocator, VkFence *pFence) {
	(void)pAllocator;

	if (!device || !pFence) {
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	VkFence fence = wgvk_alloc(sizeof(struct VkFence_T));
	if (!fence) {
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	wgvk_object_init(&fence->base, destroy_fence);
	fence->device = device;
	fence->signaled = (pCreateInfo && (pCreateInfo->flags & 0x00000001)) ? VK_TRUE : VK_FALSE;

	*pFence = fence;
	return VK_SUCCESS;
}

void vkDestroyFence(VkDevice device, VkFence fence, const VkAllocationCallbacks *pAllocator) {
	(void)device;
	(void)pAllocator;

	if (fence) {
		wgvk_object_release(&fence->base);
	}
}

VkResult vkResetFences(VkDevice device, uint32_t fenceCount, const VkFence *pFences) {
	(void)device;

	if (!pFences) {
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	for (uint32_t i = 0; i < fenceCount; i++) {
		if (pFences[i]) {
			pFences[i]->signaled = VK_FALSE;
		}
	}

	return VK_SUCCESS;
}

VkResult vkGetFenceStatus(VkDevice device, VkFence fence) {
	(void)device;

	if (!fence) {
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	return fence->signaled ? VK_SUCCESS : VK_NOT_READY;
}

VkResult vkWaitForFences(VkDevice device, uint32_t fenceCount, const VkFence *pFences,
                         VkBool32 waitAll, uint64_t timeout) {
	(void)device;
	(void)timeout;

	if (!pFences || fenceCount == 0) {
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	// WebGPU is async, so we simulate fence completion
	// In a real implementation, this would integrate with WebGPU's async model

	VkBool32 all_signaled = VK_TRUE;
	VkBool32 any_signaled = VK_FALSE;

	for (uint32_t i = 0; i < fenceCount; i++) {
		if (pFences[i]) {
			if (pFences[i]->signaled) {
				any_signaled = VK_TRUE;
			} else {
				all_signaled = VK_FALSE;
			}
		}
	}

	if (waitAll) {
		if (all_signaled) {
			return VK_SUCCESS;
		}
		// In WebGPU, we would typically return immediately or wait on callbacks
		// For now, signal all fences to simulate completion
		for (uint32_t i = 0; i < fenceCount; i++) {
			if (pFences[i]) {
				pFences[i]->signaled = VK_TRUE;
			}
		}
		return VK_SUCCESS;
	} else {
		if (any_signaled) {
			return VK_SUCCESS;
		}
		// Signal first fence as completed
		if (pFences[0]) {
			pFences[0]->signaled = VK_TRUE;
		}
		return VK_SUCCESS;
	}
}
