#include "webvulkan_internal.h"

static void destroy_fence(void* obj) {
    VkFence fence = (VkFence)obj;
    free(fence);
}

VkResult vkCreateFence(
    VkDevice device,
    const void* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkFence* pFence)
{
    (void)pCreateInfo;
    (void)pAllocator;
    
    if (!device || !pFence) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    VkFence fence = wgvk_alloc(sizeof(struct VkFence_T));
    if (!fence) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    
    wgvk_object_init(&fence->base, destroy_fence);
    fence->device = device;
    fence->signaled = false;
    
    *pFence = fence;
    return VK_SUCCESS;
}

void vkDestroyFence(
    VkDevice device,
    VkFence fence,
    const VkAllocationCallbacks* pAllocator)
{
    (void)device;
    (void)pAllocator;
    if (fence) {
        wgvk_object_release(&fence->base);
    }
}

VkResult vkResetFences(
    VkDevice device,
    uint32_t fenceCount,
    const VkFence* pFences)
{
    (void)device;
    
    for (uint32_t i = 0; i < fenceCount; i++) {
        if (pFences[i]) {
            pFences[i]->signaled = false;
        }
    }
    
    return VK_SUCCESS;
}

VkResult vkGetFenceStatus(
    VkDevice device,
    VkFence fence)
{
    (void)device;
    
    if (!fence) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    return fence->signaled ? VK_SUCCESS : VK_NOT_READY;
}

VkResult vkWaitForFences(
    VkDevice device,
    uint32_t fenceCount,
    const VkFence* pFences,
    VkBool32 waitAll,
    uint64_t timeout)
{
    (void)device;
    (void)timeout;
    
    if (!pFences || fenceCount == 0) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    if (waitAll) {
        for (uint32_t i = 0; i < fenceCount; i++) {
            if (pFences[i] && !pFences[i]->signaled) {
                return VK_TIMEOUT;
            }
        }
        return VK_SUCCESS;
    } else {
        for (uint32_t i = 0; i < fenceCount; i++) {
            if (pFences[i] && pFences[i]->signaled) {
                return VK_SUCCESS;
            }
        }
        return VK_TIMEOUT;
    }
}

static void destroy_semaphore(void* obj) {
    VkSemaphore semaphore = (VkSemaphore)obj;
    free(semaphore);
}

VkResult vkCreateSemaphore(
    VkDevice device,
    const void* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSemaphore* pSemaphore)
{
    (void)pCreateInfo;
    (void)pAllocator;
    
    if (!device || !pSemaphore) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    VkSemaphore semaphore = wgvk_alloc(sizeof(struct VkSemaphore_T));
    if (!semaphore) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    
    wgvk_object_init(&semaphore->base, destroy_semaphore);
    semaphore->device = device;
    semaphore->value = 0;
    semaphore->signaled = false;
    
    *pSemaphore = semaphore;
    return VK_SUCCESS;
}

void vkDestroySemaphore(
    VkDevice device,
    VkSemaphore semaphore,
    const VkAllocationCallbacks* pAllocator)
{
    (void)device;
    (void)pAllocator;
    if (semaphore) {
        wgvk_object_release(&semaphore->base);
    }
}

static void destroy_event(void* obj) {
    VkEvent event = (VkEvent)obj;
    free(event);
}

VkResult vkCreateEvent(
    VkDevice device,
    const void* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkEvent* pEvent)
{
    (void)pCreateInfo;
    (void)pAllocator;
    
    if (!device || !pEvent) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    VkEvent event = wgvk_alloc(sizeof(struct VkEvent_T));
    if (!event) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    
    wgvk_object_init(&event->base, destroy_event);
    event->device = device;
    event->signaled = false;
    
    *pEvent = event;
    return VK_SUCCESS;
}

void vkDestroyEvent(
    VkDevice device,
    VkEvent event,
    const VkAllocationCallbacks* pAllocator)
{
    (void)device;
    (void)pAllocator;
    if (event) {
        wgvk_object_release(&event->base);
    }
}

VkResult vkSetEvent(
    VkDevice device,
    VkEvent event)
{
    (void)device;
    
    if (!event) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    event->signaled = true;
    return VK_SUCCESS;
}

VkResult vkResetEvent(
    VkDevice device,
    VkEvent event)
{
    (void)device;
    
    if (!event) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    event->signaled = false;
    return VK_SUCCESS;
}

VkResult vkGetEventStatus(
    VkDevice device,
    VkEvent event)
{
    (void)device;
    
    if (!event) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    return event->signaled ? VK_EVENT_SET : VK_EVENT_RESET;
}
