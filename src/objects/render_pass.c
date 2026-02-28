#include "webvulkan_internal.h"

static void destroy_render_pass(void* obj) {
    VkRenderPass pass = (VkRenderPass)obj;
    wgvk_free(pass);
}

VkResult vkCreateRenderPass(
    VkDevice device,
    const VkRenderPassCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkRenderPass* pRenderPass)
{
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
    pass->depth_stencil_index = UINT32_MAX;
    pass->sample_count = 1;
    
    for (int i = 0; i < WGVK_MAX_COLOR_ATTACHMENTS; i++) {
        pass->color_formats[i] = 0;
    }
    
    if (pCreateInfo && pCreateInfo->pAttachments) {
        pass->attachment_count = pCreateInfo->attachmentCount;
        
        const VkAttachmentDescription* attachments = pCreateInfo->pAttachments;
        
        uint32_t color_idx = 0;
        for (uint32_t i = 0; i < pCreateInfo->attachmentCount; i++) {
            uint32_t fmt = attachments[i].format;
            pass->sample_count = attachments[i].samples;
            
            if ((fmt >= VK_FORMAT_D16_UNORM && fmt <= VK_FORMAT_D32_SFLOAT_S8_UINT)) {
                pass->depth_stencil_format = fmt;
                pass->depth_stencil_index = i;
            } else if (color_idx < WGVK_MAX_COLOR_ATTACHMENTS) {
                pass->color_formats[color_idx++] = fmt;
            }
        }
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
