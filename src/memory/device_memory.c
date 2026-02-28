#include "webvulkan_internal.h"

static void destroy_device_memory(void *obj) {
	VkDeviceMemory mem = (VkDeviceMemory)obj;
	if (mem->wgpu_buffer) {
		wgpuBufferRelease(mem->wgpu_buffer);
	}
	wgvk_free(mem);
}

static uint32_t vk_format_bytes_per_pixel(uint32_t format) {
	/* Covers the most common VkFormat values.  Unknown formats fall back to 4. */
	switch (format) {
	case VK_FORMAT_R8_UNORM:
	case VK_FORMAT_R8_SNORM:
	case VK_FORMAT_R8_UINT:
	case VK_FORMAT_R8_SINT:
		return 1;
	case VK_FORMAT_R8G8_UNORM:
	case VK_FORMAT_R8G8_SNORM:
	case VK_FORMAT_R8G8_UINT:
	case VK_FORMAT_R8G8_SINT:
		return 2;
	case VK_FORMAT_R8G8B8_UNORM:
	case VK_FORMAT_R8G8B8_SNORM:
	case VK_FORMAT_R8G8B8_UINT:
	case VK_FORMAT_R8G8B8_SINT:
		return 3;
	case VK_FORMAT_R8G8B8A8_UNORM:
	case VK_FORMAT_R8G8B8A8_SNORM:
	case VK_FORMAT_R8G8B8A8_UINT:
	case VK_FORMAT_R8G8B8A8_SINT:
	case VK_FORMAT_B8G8R8A8_UNORM:
	case VK_FORMAT_B8G8R8A8_SNORM:
	case VK_FORMAT_B8G8R8A8_UINT:
	case VK_FORMAT_B8G8R8A8_SINT:
		return 4;
	case VK_FORMAT_R16_UNORM:
	case VK_FORMAT_R16_SNORM:
	case VK_FORMAT_R16_UINT:
	case VK_FORMAT_R16_SINT:
		return 2;
	case VK_FORMAT_R16G16_UNORM:
	case VK_FORMAT_R16G16_SNORM:
	case VK_FORMAT_R16G16_UINT:
	case VK_FORMAT_R16G16_SINT:
		return 4;
	case VK_FORMAT_R16G16B16_UNORM:
	case VK_FORMAT_R16G16B16_SNORM:
	case VK_FORMAT_R16G16B16_UINT:
	case VK_FORMAT_R16G16B16_SINT:
		return 6;
	case VK_FORMAT_R16G16B16A16_UNORM:
	case VK_FORMAT_R16G16B16A16_SNORM:
	case VK_FORMAT_R16G16B16A16_UINT:
	case VK_FORMAT_R16G16B16A16_SINT:
		return 8;
	case VK_FORMAT_R32_UINT:
	case VK_FORMAT_R32_SINT:
	case VK_FORMAT_R32_SFLOAT:
		return 4;
	case VK_FORMAT_R32G32_UINT:
	case VK_FORMAT_R32G32_SINT:
	case VK_FORMAT_R32G32_SFLOAT:
		return 8;
	case VK_FORMAT_R32G32B32_UINT:
	case VK_FORMAT_R32G32B32_SINT:
	case VK_FORMAT_R32G32B32_SFLOAT:
		return 12;
	case VK_FORMAT_R32G32B32A32_UINT:
	case VK_FORMAT_R32G32B32A32_SINT:
	case VK_FORMAT_R32G32B32A32_SFLOAT:
		return 16;
	case VK_FORMAT_D16_UNORM:
		return 2;
	case VK_FORMAT_X8_D24_UNORM_PACK32:
	case VK_FORMAT_D32_SFLOAT:
	case VK_FORMAT_S8_UINT:
		return 4;
	case VK_FORMAT_D24_UNORM_S8_UINT:
	case VK_FORMAT_D32_SFLOAT_S8_UINT:
		return 5;
	default:
		return 4;
	}
}

VkResult vkAllocateMemory(VkDevice device, const VkMemoryAllocateInfo *pAllocateInfo,
                          const VkAllocationCallbacks *pAllocator, VkDeviceMemory *pMemory) {
	(void)pAllocator;

	if (!device || !pMemory) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	VkDeviceMemory mem = wgvk_alloc(sizeof(struct VkDeviceMemory_T));
	if (!mem) {
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	wgvk_object_init(&mem->base, destroy_device_memory);
	mem->device = device;
	mem->size = pAllocateInfo->allocationSize;
	mem->memory_type_index = pAllocateInfo->memoryTypeIndex;
	mem->mapped_ptr = NULL;
	mem->wgpu_buffer = NULL;

	WGPUBufferDescriptor desc = {
	    .size = pAllocateInfo->allocationSize,
	    .usage = WGPUBufferUsage_CopySrc | WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform |
	             WGPUBufferUsage_Storage | WGPUBufferUsage_Vertex | WGPUBufferUsage_Index,
	    .mappedAtCreation = VK_FALSE,
	};

	mem->wgpu_buffer = wgpuDeviceCreateBuffer(device->wgpu_device, &desc);

	if (!mem->wgpu_buffer) {
		wgvk_free(mem);
		return VK_ERROR_OUT_OF_DEVICE_MEMORY;
	}

	*pMemory = mem;
	return VK_SUCCESS;
}

void vkFreeMemory(VkDevice device, VkDeviceMemory memory, const VkAllocationCallbacks *pAllocator) {
	(void)device;
	(void)pAllocator;
	if (memory) {
		wgvk_object_release(&memory->base);
	}
}

VkResult vkMapMemory(VkDevice device, VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size,
                     VkFlags flags, void **ppData) {
	(void)flags;
	(void)size; /* We map the full allocation; offset is applied to the returned pointer */

	if (!memory || !ppData) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	if (!memory->wgpu_buffer) {
		return VK_ERROR_MEMORY_MAP_FAILED;
	}

	if (memory->mapped_ptr) {
		/* Already mapped — return existing pointer */
		*ppData = (uint8_t *)memory->mapped_ptr + offset;
		return VK_SUCCESS;
	}

	/* Allocate a host-side shadow buffer.  On vkUnmapMemory we upload via
	 * wgpuQueueWriteBuffer.  This is the only portable approach across both
	 * Emscripten (no synchronous map) and native Dawn. */
	memory->mapped_ptr = malloc((size_t)memory->size);
	if (!memory->mapped_ptr) {
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	*ppData = (uint8_t *)memory->mapped_ptr + offset;
	return VK_SUCCESS;
}

void vkUnmapMemory(VkDevice device, VkDeviceMemory memory) {
	if (!memory || !memory->wgpu_buffer || !memory->mapped_ptr) {
		return;
	}

	/* Upload the shadow buffer to the GPU buffer, then free the host copy. */
	wgpuQueueWriteBuffer(device->wgpu_queue, memory->wgpu_buffer, 0, memory->mapped_ptr,
	                     (size_t)memory->size);

	free(memory->mapped_ptr);
	memory->mapped_ptr = NULL;
}

VkResult vkBindBufferMemory(VkDevice device, VkBuffer buffer, VkDeviceMemory memory,
                            VkDeviceSize memoryOffset) {
	(void)device;
	(void)memory;
	(void)memoryOffset;

	if (!buffer) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	buffer->bound_memory = memory;
	buffer->memory_offset = memoryOffset;

	return VK_SUCCESS;
}

VkResult vkBindImageMemory(VkDevice device, VkImage image, VkDeviceMemory memory,
                           VkDeviceSize memoryOffset) {
	(void)device;
	(void)memory;
	(void)memoryOffset;

	if (!image) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	image->bound_memory = memory;
	image->memory_offset = memoryOffset;

	return VK_SUCCESS;
}

void vkGetBufferMemoryRequirements(VkDevice device, VkBuffer buffer, void *pMemoryRequirements) {
	(void)device;
	(void)buffer;

	if (!pMemoryRequirements) {
		return;
	}

	struct {
		VkDeviceSize size;
		VkDeviceSize alignment;
		uint32_t memoryTypeBits;
	} *reqs = pMemoryRequirements;

	reqs->size = buffer ? buffer->size : 0;
	reqs->alignment = 256;
	reqs->memoryTypeBits = 0x1;
}

void vkGetImageMemoryRequirements(VkDevice device, VkImage image, void *pMemoryRequirements) {
	(void)device;

	if (!pMemoryRequirements) {
		return;
	}

	struct {
		VkDeviceSize size;
		VkDeviceSize alignment;
		uint32_t memoryTypeBits;
	} *reqs = pMemoryRequirements;

	if (!image) {
		reqs->size = 0;
		reqs->alignment = 256;
		reqs->memoryTypeBits = 0x1;
		return;
	}

	/* Sum all mip levels: each level halves width and height. */
	uint32_t bpp = vk_format_bytes_per_pixel(image->format);
	uint32_t depth = (image->depth > 0) ? image->depth : 1;
	uint32_t layers = (image->array_layers > 0) ? image->array_layers : 1;
	uint32_t mips = (image->mip_levels > 0) ? image->mip_levels : 1;
	uint32_t w = (image->width > 0) ? image->width : 1;
	uint32_t h = (image->height > 0) ? image->height : 1;

	VkDeviceSize total = 0;
	for (uint32_t m = 0; m < mips; m++) {
		uint32_t mw = (w >> m) > 0 ? (w >> m) : 1;
		uint32_t mh = (h >> m) > 0 ? (h >> m) : 1;
		total += (VkDeviceSize)mw * mh * depth * layers * bpp;
	}
	/* Round up to 256-byte alignment. */
	total = (total + 255) & ~(VkDeviceSize)255;

	reqs->size = total;
	reqs->alignment = 256;
	reqs->memoryTypeBits = 0x1;
}
