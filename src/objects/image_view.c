#include "webvulkan_internal.h"

static void destroy_image_view(void *obj) {
	VkImageView view = (VkImageView)obj;
	if (view->wgpu_view) {
		wgpuTextureViewRelease(view->wgpu_view);
	}
	wgvk_free(view);
}

VkResult vkCreateImageView(VkDevice device, const VkImageViewCreateInfo *pCreateInfo,
                           const VkAllocationCallbacks *pAllocator, VkImageView *pView) {
	(void)pAllocator;

	if (!device || !pCreateInfo || !pView) {
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	VkImageView view = wgvk_alloc(sizeof(struct VkImageView_T));
	if (!view) {
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	wgvk_object_init(&view->base, destroy_image_view);
	view->device = device;
	view->image = (VkImage)pCreateInfo->image;
	view->view_type = pCreateInfo->viewType;
	view->format = pCreateInfo->format;
	view->wgpu_view = NULL;

	if (view->image && view->image->wgpu_texture) {
		WGPUTextureViewDescriptor desc = {0};
		desc.format = wgpuTextureGetFormat(view->image->wgpu_texture);
		desc.dimension = WGPUTextureViewDimension_2D;
		desc.baseMipLevel = 0;
		desc.mipLevelCount = view->image->mip_levels;
		desc.baseArrayLayer = 0;
		desc.arrayLayerCount = view->image->array_layers;

		view->wgpu_view = wgpuTextureCreateView(view->image->wgpu_texture, &desc);
	}

	*pView = view;
	return VK_SUCCESS;
}

void vkDestroyImageView(VkDevice device, VkImageView imageView,
                        const VkAllocationCallbacks *pAllocator) {
	(void)device;
	(void)pAllocator;

	if (imageView) {
		wgvk_object_release(&imageView->base);
	}
}
