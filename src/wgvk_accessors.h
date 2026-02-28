#ifndef WGVK_ACCESSORS_H
#define WGVK_ACCESSORS_H

#include <webgpu/webgpu.h>
#include "vulkan_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

WGPURenderPipeline wgvk_pipeline_get_render(VkPipeline pipeline);
WGPUBuffer wgvk_buffer_get_wgpu(VkBuffer buffer);

#ifdef __cplusplus
}
#endif

#endif
