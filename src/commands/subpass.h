#ifndef WGVK_SUBPASS_H
#define WGVK_SUBPASS_H

#include "webvulkan_internal.h"

typedef struct {
    uint32_t subpass_index;
    uint32_t input_attachment_count;
    uint32_t input_attachments[8];
    uint32_t color_attachment_count;
    uint32_t color_attachments[8];
    uint32_t depth_stencil_attachment;
    bool has_depth_stencil;
} WgvkSubpassInfo;

typedef struct {
    uint32_t subpass_count;
    WgvkSubpassInfo subpasses[8];
    uint32_t attachment_count;
    WGPUTextureFormat attachment_formats[16];
} WgvkRenderPassInfo;

void wgvk_subpass_analyze(VkRenderPass renderPass, WgvkRenderPassInfo* info);
void wgvk_subpass_begin(WGPUCommandEncoder encoder, VkRenderPass renderPass, VkFramebuffer framebuffer, uint32_t subpass);
void wgvk_subpass_end(WGPUCommandEncoder encoder);
uint32_t wgvk_subpass_get_count(VkRenderPass renderPass);

#endif
