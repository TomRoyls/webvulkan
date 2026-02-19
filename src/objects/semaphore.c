#include "webvulkan_internal.h"

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
    semaphore->signaled = VK_FALSE;
    
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
