#include "webvulkan_internal.h"

static void destroy_semaphore(void *obj) {
	VkSemaphore semaphore = (VkSemaphore)obj;
	wgvk_free(semaphore);
}

VkResult vkCreateSemaphore(VkDevice device, const void *pCreateInfo,
                           const VkAllocationCallbacks *pAllocator, VkSemaphore *pSemaphore) {
	(void)pAllocator;

	if (!device || !pSemaphore) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	VkSemaphore semaphore = wgvk_alloc(sizeof(struct VkSemaphore_T));
	if (!semaphore) {
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	VkBool32 is_timeline = VK_FALSE;
	uint64_t initial_value = 0;

	if (pCreateInfo) {
		struct VkSemaphoreCreateInfo {
			VkStructureType sType;
			const void *pNext;
			uint32_t flags;
		} const *create_info = pCreateInfo;

		const void *next = create_info->pNext;
		while (next) {
			struct {
				VkStructureType sType;
				const void *pNext;
			} const *header = next;

			if (header->sType == VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO) {
				struct {
					VkStructureType sType;
					const void *pNext;
					uint32_t semaphoreType;
					uint64_t initialValue;
				} const *type_info = next;
				is_timeline = (type_info->semaphoreType == 1);
				initial_value = type_info->initialValue;
				break;
			}
			next = header->pNext;
		}
	}

	wgvk_object_init(&semaphore->base, destroy_semaphore);
	semaphore->device = device;
	semaphore->value = initial_value;
	semaphore->signaled = is_timeline ? VK_TRUE : VK_FALSE;
	semaphore->timeline = is_timeline;

	*pSemaphore = semaphore;
	return VK_SUCCESS;
}

void vkDestroySemaphore(VkDevice device, VkSemaphore semaphore,
                        const VkAllocationCallbacks *pAllocator) {
	(void)device;
	(void)pAllocator;
	if (semaphore) {
		wgvk_object_release(&semaphore->base);
	}
}

VkResult vkGetSemaphoreCounterValue(VkDevice device, VkSemaphore semaphore, uint64_t *pValue) {
	(void)device;

	if (!semaphore || !pValue) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	*pValue = semaphore->value;
	return VK_SUCCESS;
}

VkResult vkSignalSemaphore(VkDevice device, const void *pSignalInfo) {
	(void)device;

	if (!pSignalInfo) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	struct {
		VkStructureType sType;
		const void *pNext;
		VkSemaphore semaphore;
		uint64_t value;
	} const *signal_info = pSignalInfo;

	if (signal_info->semaphore) {
		signal_info->semaphore->value = signal_info->value;
		signal_info->semaphore->signaled = VK_TRUE;
	}

	return VK_SUCCESS;
}

VkResult vkWaitSemaphores(VkDevice device, const void *pWaitInfo, uint64_t timeout) {
	(void)device;
	(void)timeout;

	if (!pWaitInfo) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	struct {
		VkStructureType sType;
		const void *pNext;
		uint32_t flags;
		uint32_t semaphoreCount;
		const VkSemaphore *pSemaphores;
		const uint64_t *pValues;
	} const *wait_info = pWaitInfo;

	for (uint32_t i = 0; i < wait_info->semaphoreCount; i++) {
		VkSemaphore sem = wait_info->pSemaphores[i];
		uint64_t wait_value = wait_info->pValues[i];

		if (sem && sem->value < wait_value) {
			sem->value = wait_value;
		}
	}

	return VK_SUCCESS;
}
