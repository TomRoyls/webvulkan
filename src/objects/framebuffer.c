#include "webvulkan_internal.h"

static void destroy_framebuffer(void* obj) {
    VkFramebuffer fb = (VkFramebuffer)obj;
    for (uint32_t i = 0; i < fb->attachment_count; i++) {
        if (fb->attachments[i]) {
            wgvk_object_release(&fb->attachments[i]->base);
        }
    }
    wgvk_free(fb);
}

VkResult vkCreateFramebuffer(
    VkDevice device,
    const VkFramebufferCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkFramebuffer* pFramebuffer) {
    
    (void)pAllocator;
    
    if (!device || !pCreateInfo || !pFramebuffer) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    
    VkFramebuffer fb = wgvk_alloc(sizeof(struct VkFramebuffer_T));
    if (!fb) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    
    wgvk_object_init(&fb->base, destroy_framebuffer);
    fb->device = device;
    fb->render_pass = pCreateInfo->renderPass;
    fb->width = pCreateInfo->width;
    fb->height = pCreateInfo->height;
    fb->layers = pCreateInfo->layers;
    fb->attachment_count = pCreateInfo->attachmentCount;
    
    for (uint32_t i = 0; i < pCreateInfo->attachmentCount && i < WGVK_MAX_COLOR_ATTACHMENTS + 1; i++) {
        fb->attachments[i] = (VkImageView)pCreateInfo->pAttachments[i];
        if (fb->attachments[i]) {
            wgvk_object_retain(&fb->attachments[i]->base);
        }
    }
    
    *pFramebuffer = fb;
    return VK_SUCCESS;
}

void vkDestroyFramebuffer(
    VkDevice device,
    VkFramebuffer framebuffer,
    const VkAllocationCallbacks* pAllocator) {
    
    (void)device;
    (void)pAllocator;
    
    if (framebuffer) {
        wgvk_object_release(&framebuffer->base);
    }
}
