#include "webvulkan_internal.h"

static void destroy_command_pool(void *obj) {
	VkCommandPool pool = (VkCommandPool)obj;
	wgvk_free(pool);
}

VkResult vkCreateCommandPool(VkDevice device, const VkCommandPoolCreateInfo *pCreateInfo,
                             const VkAllocationCallbacks *pAllocator, VkCommandPool *pCommandPool) {
	(void)pAllocator;

	if (!device || !pCreateInfo || !pCommandPool) {
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	VkCommandPool pool = wgvk_alloc(sizeof(struct VkCommandPool_T));
	if (!pool) {
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	wgvk_object_init(&pool->base, destroy_command_pool);
	pool->device = device;
	pool->queue_family_index = pCreateInfo->queueFamilyIndex;

	*pCommandPool = pool;
	return VK_SUCCESS;
}

void vkDestroyCommandPool(VkDevice device, VkCommandPool commandPool,
                          const VkAllocationCallbacks *pAllocator) {
	(void)device;
	(void)pAllocator;

	if (commandPool) {
		wgvk_object_release(&commandPool->base);
	}
}

VkResult vkResetCommandPool(VkDevice device, VkCommandPool commandPool, VkFlags flags) {
	(void)device;
	(void)commandPool;
	(void)flags;

	return VK_SUCCESS;
}
