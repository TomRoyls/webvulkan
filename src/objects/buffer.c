#include "webvulkan_internal.h"

static void destroy_buffer(void *obj) {
	VkBuffer buffer = (VkBuffer)obj;
	if (buffer->wgpu_buffer) {
		wgpuBufferRelease(buffer->wgpu_buffer);
	}
	wgvk_free(buffer);
}

VkResult vkCreateBuffer(VkDevice device, const VkBufferCreateInfo *pCreateInfo,
                        const VkAllocationCallbacks *pAllocator, VkBuffer *pBuffer) {
	(void)pAllocator;

	if (!device || !pCreateInfo || !pBuffer) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	VkBuffer buffer = wgvk_alloc(sizeof(struct VkBuffer_T));
	if (!buffer) {
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	wgvk_object_init(&buffer->base, destroy_buffer);
	buffer->device = device;
	buffer->size = pCreateInfo->size;
	buffer->usage = pCreateInfo->usage;
	buffer->bound_memory = NULL;
	buffer->memory_offset = 0;
	buffer->wgpu_buffer = NULL;

	WGPUBufferUsage wgpu_usage = WGPUBufferUsage_None;
	if (pCreateInfo->usage & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
		wgpu_usage |= WGPUBufferUsage_Vertex;
	if (pCreateInfo->usage & VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
		wgpu_usage |= WGPUBufferUsage_Index;
	if (pCreateInfo->usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
		wgpu_usage |= WGPUBufferUsage_Uniform;
	if (pCreateInfo->usage & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)
		wgpu_usage |= WGPUBufferUsage_Storage;
	if (pCreateInfo->usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
		wgpu_usage |= WGPUBufferUsage_CopySrc;
	if (pCreateInfo->usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT)
		wgpu_usage |= WGPUBufferUsage_CopyDst;
	if (pCreateInfo->usage & VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT)
		wgpu_usage |= WGPUBufferUsage_Indirect;

	WGPUBufferDescriptor desc = {
	    .size = pCreateInfo->size,
	    .usage = wgpu_usage,
	    .mappedAtCreation = VK_FALSE,
	};

	buffer->wgpu_buffer = wgpuDeviceCreateBuffer(device->wgpu_device, &desc);
	if (!buffer->wgpu_buffer) {
		wgvk_free(buffer);
		return VK_ERROR_OUT_OF_DEVICE_MEMORY;
	}

	*pBuffer = buffer;
	return VK_SUCCESS;
}

void vkDestroyBuffer(VkDevice device, VkBuffer buffer, const VkAllocationCallbacks *pAllocator) {
	(void)device;
	(void)pAllocator;
	if (buffer) {
		wgvk_object_release(&buffer->base);
	}
}

WGPUBuffer wgvk_buffer_get_wgpu(VkBuffer buffer) {
	return buffer ? buffer->wgpu_buffer : NULL;
}
