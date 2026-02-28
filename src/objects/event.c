#include "webvulkan_internal.h"

static void destroy_event(void *obj) {
	VkEvent event = (VkEvent)obj;
	wgvk_free(event);
}

VkResult vkCreateEvent(VkDevice device, const void *pCreateInfo,
                       const VkAllocationCallbacks *pAllocator, VkEvent *pEvent) {
	(void)pCreateInfo;
	(void)pAllocator;

	if (!device || !pEvent) {
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	VkEvent event = wgvk_alloc(sizeof(struct VkEvent_T));
	if (!event) {
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	wgvk_object_init(&event->base, destroy_event);
	event->device = device;
	event->signaled = VK_FALSE;

	*pEvent = event;
	return VK_SUCCESS;
}

void vkDestroyEvent(VkDevice device, VkEvent event, const VkAllocationCallbacks *pAllocator) {
	(void)device;
	(void)pAllocator;

	if (event) {
		wgvk_object_release(&event->base);
	}
}

VkResult vkSetEvent(VkDevice device, VkEvent event) {
	(void)device;

	if (!event) {
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	event->signaled = VK_TRUE;
	return VK_SUCCESS;
}

VkResult vkResetEvent(VkDevice device, VkEvent event) {
	(void)device;

	if (!event) {
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	event->signaled = VK_FALSE;
	return VK_SUCCESS;
}

VkResult vkGetEventStatus(VkDevice device, VkEvent event) {
	(void)device;

	if (!event) {
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	return event->signaled ? VK_EVENT_SET : VK_EVENT_RESET;
}
