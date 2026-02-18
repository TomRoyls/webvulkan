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
