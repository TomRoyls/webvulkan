#include "webvulkan_internal.h"

static void destroy_render_pass(void* obj) {
    VkRenderPass pass = (VkRenderPass)obj;
    free(pass);
}

VkResult vkCreateRenderPass(
    VkDevice device,
    const VkRenderPassCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkRenderPass* pRenderPass)
{
    (void)pCreateInfo;
    (void)pAllocator;
    
    if (!device || !pRenderPass) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    VkRenderPass pass = wgvk_alloc(sizeof(struct VkRenderPass_T));
    if (!pass) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    
    wgvk_object_init(&pass->base, destroy_render_pass);
    pass->device = device;
    pass->attachment_count = 0;
    pass->depth_stencil_format = 0;
    pass->sample_count = 1;
    
    for (int i = 0; i < WGVK_MAX_COLOR_ATTACHMENTS; i++) {
        pass->color_formats[i] = 0;
    }
    
    *pRenderPass = pass;
    return VK_SUCCESS;
}

void vkDestroyRenderPass(
    VkDevice device,
    VkRenderPass renderPass,
    const VkAllocationCallbacks* pAllocator)
{
    (void)device;
    (void)pAllocator;
    if (renderPass) {
        wgvk_object_release(&renderPass->base);
    }
}

static void destroy_framebuffer(void* obj) {
    VkFramebuffer fb = (VkFramebuffer)obj;
    free(fb);
}

VkResult vkCreateFramebuffer(
    VkDevice device,
    const VkFramebufferCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkFramebuffer* pFramebuffer)
{
    (void)pAllocator;
    
    if (!device || !pCreateInfo || !pFramebuffer) {
        return VK_ERROR_INITIALIZATION_FAILED;
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
        fb->attachments[i] = pCreateInfo->pAttachments[i];
    }
    
    *pFramebuffer = fb;
    return VK_SUCCESS;
}

void vkDestroyFramebuffer(
    VkDevice device,
    VkFramebuffer framebuffer,
    const VkAllocationCallbacks* pAllocator)
{
    (void)device;
    (void)pAllocator;
    if (framebuffer) {
        wgvk_object_release(&framebuffer->base);
    }
}
