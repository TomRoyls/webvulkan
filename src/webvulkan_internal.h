#ifndef WEBVULKAN_INTERNAL_H
#define WEBVULKAN_INTERNAL_H

#define VK_NO_PROTOTYPES
#include "vulkan_platform.h"
#include <vulkan/vulkan_core.h>
#include <webgpu/webgpu.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define WGVK_MAX_BIND_GROUPS 4
#define WGVK_MAX_VERTEX_BUFFERS 16
#define WGVK_MAX_COLOR_ATTACHMENTS 8
#define WGVK_PUSH_CONSTANT_SIZE 128

struct WgvkObject {
    volatile int32_t ref_count;
    void (*destroy)(void* obj);
};

static inline void wgvk_object_init(struct WgvkObject* obj, void (*destroy)(void*)) {
    obj->ref_count = 1;
    obj->destroy = destroy;
}

static inline void wgvk_object_retain(struct WgvkObject* obj) {
    __sync_fetch_and_add(&obj->ref_count, 1);
}

static inline void wgvk_object_release(struct WgvkObject* obj) {
    if (__sync_sub_and_fetch(&obj->ref_count, 1) == 0) {
        if (obj->destroy) {
            obj->destroy(obj);
        }
    }
}

struct VkInstance_T {
    struct WgvkObject base;
    WGPUInstance wgpu_instance;
    uint32_t api_version;
    char* application_name;
    char* engine_name;
};

struct VkPhysicalDevice_T {
    struct WgvkObject base;
    VkInstance instance;
    WGPUAdapter wgpu_adapter;
    WGPUAdapterInfo adapter_info;
    uint32_t api_version;
};

struct VkDevice_T {
    struct WgvkObject base;
    VkPhysicalDevice physical_device;
    WGPUDevice wgpu_device;
    WGPUQueue wgpu_queue;
    WGPUBuffer push_constant_buffer;
    uint32_t queue_family_index;
};

struct VkQueue_T {
    struct WgvkObject base;
    VkDevice device;
    WGPUQueue wgpu_queue;
    uint32_t queue_family_index;
    uint32_t queue_index;
};

struct VkDeviceMemory_T {
    struct WgvkObject base;
    VkDevice device;
    VkDeviceSize size;
    uint32_t memory_type_index;
    void* mapped_ptr;
    WGPUBuffer wgpu_buffer;
};

struct VkBuffer_T {
    struct WgvkObject base;
    VkDevice device;
    WGPUBuffer wgpu_buffer;
    VkDeviceSize size;
    VkBufferUsageFlags usage;
    VkDeviceMemory bound_memory;
    VkDeviceSize memory_offset;
};

struct VkImage_T {
    struct WgvkObject base;
    VkDevice device;
    WGPUTexture wgpu_texture;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t mip_levels;
    uint32_t array_layers;
    uint32_t format;
    uint32_t image_type;
    uint32_t samples;
    VkImageUsageFlags usage;
    VkDeviceMemory bound_memory;
    VkDeviceSize memory_offset;
};

struct VkImageView_T {
    struct WgvkObject base;
    VkDevice device;
    VkImage image;
    WGPUTextureView wgpu_view;
    uint32_t view_type;
    uint32_t format;
};

struct VkShaderModule_T {
    struct WgvkObject base;
    VkDevice device;
    WGPUShaderModule wgpu_shader;
    char* wgsl_source;
    uint32_t* spirv_code;
    size_t spirv_size;
};

struct VkPipelineLayout_T {
    struct WgvkObject base;
    VkDevice device;
    WGPUPipelineLayout wgpu_layout;
    uint32_t set_layout_count;
    VkDescriptorSetLayout set_layouts[WGVK_MAX_BIND_GROUPS];
    uint32_t push_constant_size;
};

struct VkPipeline_T {
    struct WgvkObject base;
    VkDevice device;
    VkPipelineLayout layout;
    union {
        WGPURenderPipeline render;
        WGPUComputePipeline compute;
    } wgpu_pipeline;
    VkPipelineBindPoint bind_point;
};

struct VkRenderPass_T {
    struct WgvkObject base;
    VkDevice device;
    uint32_t attachment_count;
    uint32_t color_formats[WGVK_MAX_COLOR_ATTACHMENTS];
    uint32_t depth_stencil_format;
    uint32_t sample_count;
};

struct VkFramebuffer_T {
    struct WgvkObject base;
    VkDevice device;
    VkRenderPass render_pass;
    uint32_t width;
    uint32_t height;
    uint32_t layers;
    uint32_t attachment_count;
    VkImageView attachments[WGVK_MAX_COLOR_ATTACHMENTS + 1];
};

struct VkCommandPool_T {
    struct WgvkObject base;
    VkDevice device;
    uint32_t queue_family_index;
};

struct VkCommandBuffer_T {
    struct WgvkObject base;
    VkDevice device;
    VkCommandPool pool;
    WGPUCommandEncoder wgpu_encoder;
    WGPURenderPassEncoder wgpu_render_pass;
    WGPUComputePassEncoder wgpu_compute_pass;
    VkCommandBufferLevel level;
    VkBool32 recording;
    VkBool32 in_render_pass;
    VkBool32 in_compute_pass;
    uint32_t push_constant_offset;
    
    VkPipeline bound_pipeline;
    VkPipelineLayout bound_layout;
    VkBuffer bound_vertex_buffers[WGVK_MAX_VERTEX_BUFFERS];
    VkDeviceSize bound_vertex_offsets[WGVK_MAX_VERTEX_BUFFERS];
    VkBuffer bound_index_buffer;
    VkDeviceSize bound_index_offset;
    VkIndexType bound_index_type;
    VkDescriptorSet bound_descriptor_sets[WGVK_MAX_BIND_GROUPS];
};

struct VkSemaphore_T {
    struct WgvkObject base;
    VkDevice device;
    uint64_t value;
    VkBool32 signaled;
};

struct VkFence_T {
    struct WgvkObject base;
    VkDevice device;
    VkBool32 signaled;
};

struct VkEvent_T {
    struct WgvkObject base;
    VkDevice device;
    VkBool32 signaled;
};

struct VkDescriptorSetLayout_T {
    struct WgvkObject base;
    VkDevice device;
    WGPUBindGroupLayout wgpu_layout;
    uint32_t binding_count;
};

struct VkDescriptorPool_T {
    struct WgvkObject base;
    VkDevice device;
};

struct VkDescriptorSet_T {
    struct WgvkObject base;
    VkDevice device;
    VkDescriptorSetLayout layout;
    WGPUBindGroup wgpu_bind_group;
};

struct VkSampler_T {
    struct WgvkObject base;
    VkDevice device;
    WGPUSampler wgpu_sampler;
};

static inline void* wgvk_alloc(size_t size) {
    void* ptr = calloc(1, size);
    return ptr;
}

static inline void wgvk_free(void* ptr) {
    free(ptr);
}

static inline char* wgvk_strdup(const char* str) {
    if (!str) return NULL;
    size_t len = strlen(str);
    char* dup = malloc(len + 1);
    if (dup) {
        memcpy(dup, str, len + 1);
    }
    return dup;
}

#endif
