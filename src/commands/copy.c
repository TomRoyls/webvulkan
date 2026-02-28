#include "webvulkan_internal.h"

void vkCmdCopyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer,
                     uint32_t regionCount, const void *pRegions) {
	if (!commandBuffer || !srcBuffer || !dstBuffer || !commandBuffer->wgpu_encoder) {
		return;
	}

	const struct {
		VkDeviceSize srcOffset;
		VkDeviceSize dstOffset;
		VkDeviceSize size;
	} *regions = pRegions;

	for (uint32_t i = 0; i < regionCount; i++) {
		wgpuCommandEncoderCopyBufferToBuffer(commandBuffer->wgpu_encoder, srcBuffer->wgpu_buffer,
		                                     regions[i].srcOffset, dstBuffer->wgpu_buffer,
		                                     regions[i].dstOffset, regions[i].size);
	}
}

void vkCmdCopyImage(VkCommandBuffer commandBuffer, VkImage srcImage, uint32_t srcImageLayout,
                    VkImage dstImage, uint32_t dstImageLayout, uint32_t regionCount,
                    const void *pRegions) {
	(void)srcImageLayout;
	(void)dstImageLayout;

	if (!commandBuffer || !srcImage || !dstImage || !commandBuffer->wgpu_encoder) {
		return;
	}

	const struct {
		uint32_t srcSubresource[5];
		uint32_t srcOffset[3];
		uint32_t dstSubresource[5];
		uint32_t dstOffset[3];
		uint32_t extent[3];
	} *regions = pRegions;

	for (uint32_t i = 0; i < regionCount; i++) {
		(void)regions;
	}
}

void vkCmdCopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkImage dstImage,
                            uint32_t dstImageLayout, uint32_t regionCount, const void *pRegions) {
	(void)dstImageLayout;

	if (!commandBuffer || !srcBuffer || !dstImage || !commandBuffer->wgpu_encoder) {
		return;
	}

	const struct {
		VkDeviceSize bufferOffset;
		uint32_t bufferRowLength;
		uint32_t bufferImageHeight;
		uint32_t imageSubresource[5];
		uint32_t imageOffset[3];
		uint32_t imageExtent[3];
	} *regions = pRegions;

	for (uint32_t i = 0; i < regionCount; i++) {
		WGPUTexelCopyBufferInfo src = {
		    .layout =
		        {
		            .offset = regions[i].bufferOffset,
		            .bytesPerRow = regions[i].bufferRowLength > 0 ? regions[i].bufferRowLength * 4
		                                                          : WGPU_COPY_STRIDE_UNDEFINED,
		            .rowsPerImage = regions[i].bufferImageHeight > 0 ? regions[i].bufferImageHeight
		                                                             : WGPU_COPY_STRIDE_UNDEFINED,
		        },
		    .buffer = srcBuffer->wgpu_buffer,
		};

		WGPUTexelCopyTextureInfo dst = {
		    .texture = dstImage->wgpu_texture,
		    .mipLevel = regions[i].imageSubresource[0],
		    .origin =
		        {
		            .x = regions[i].imageOffset[0],
		            .y = regions[i].imageOffset[1],
		            .z = regions[i].imageOffset[2],
		        },
		    .aspect = WGPUTextureAspect_All,
		};

		WGPUExtent3D size = {
		    .width = regions[i].imageExtent[0],
		    .height = regions[i].imageExtent[1],
		    .depthOrArrayLayers = regions[i].imageExtent[2],
		};

		wgpuCommandEncoderCopyBufferToTexture(commandBuffer->wgpu_encoder, &src, &dst, &size);
	}
}

void vkCmdCopyImageToBuffer(VkCommandBuffer commandBuffer, VkImage srcImage,
                            uint32_t srcImageLayout, VkBuffer dstBuffer, uint32_t regionCount,
                            const void *pRegions) {
	(void)srcImageLayout;

	if (!commandBuffer || !srcImage || !dstBuffer || !commandBuffer->wgpu_encoder) {
		return;
	}

	const struct {
		VkDeviceSize bufferOffset;
		uint32_t bufferRowLength;
		uint32_t bufferImageHeight;
		uint32_t imageSubresource[5];
		uint32_t imageOffset[3];
		uint32_t imageExtent[3];
	} *regions = pRegions;

	for (uint32_t i = 0; i < regionCount; i++) {
		WGPUTexelCopyTextureInfo src = {
		    .texture = srcImage->wgpu_texture,
		    .mipLevel = regions[i].imageSubresource[0],
		    .origin =
		        {
		            .x = regions[i].imageOffset[0],
		            .y = regions[i].imageOffset[1],
		            .z = regions[i].imageOffset[2],
		        },
		    .aspect = WGPUTextureAspect_All,
		};

		WGPUTexelCopyBufferInfo dst = {
		    .layout =
		        {
		            .offset = regions[i].bufferOffset,
		            .bytesPerRow = regions[i].bufferRowLength > 0 ? regions[i].bufferRowLength * 4
		                                                          : WGPU_COPY_STRIDE_UNDEFINED,
		            .rowsPerImage = regions[i].bufferImageHeight > 0 ? regions[i].bufferImageHeight
		                                                             : WGPU_COPY_STRIDE_UNDEFINED,
		        },
		    .buffer = dstBuffer->wgpu_buffer,
		};

		WGPUExtent3D size = {
		    .width = regions[i].imageExtent[0],
		    .height = regions[i].imageExtent[1],
		    .depthOrArrayLayers = regions[i].imageExtent[2],
		};

		wgpuCommandEncoderCopyTextureToBuffer(commandBuffer->wgpu_encoder, &src, &dst, &size);
	}
}

void vkCmdBlitImage(VkCommandBuffer commandBuffer, VkImage srcImage, uint32_t srcImageLayout,
                    VkImage dstImage, uint32_t dstImageLayout, uint32_t regionCount,
                    const void *pRegions, uint32_t filter) {
	(void)srcImageLayout;
	(void)dstImageLayout;
	(void)regionCount;
	(void)pRegions;
	(void)filter;

	if (!commandBuffer || !srcImage || !dstImage) {
		return;
	}
}

void vkCmdResolveImage(VkCommandBuffer commandBuffer, VkImage srcImage, uint32_t srcImageLayout,
                       VkImage dstImage, uint32_t dstImageLayout, uint32_t regionCount,
                       const void *pRegions) {
	(void)srcImageLayout;
	(void)dstImageLayout;
	(void)regionCount;
	(void)pRegions;

	if (!commandBuffer || !srcImage || !dstImage) {
		return;
	}
}

void vkCmdFillBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset,
                     VkDeviceSize size, uint32_t data) {
	(void)data;

	if (!commandBuffer || !dstBuffer || !commandBuffer->wgpu_encoder) {
		return;
	}

	(void)dstOffset;
	(void)size;
}

void vkCmdUpdateBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset,
                       VkDeviceSize dataSize, const void *pData) {
	(void)pData;

	if (!commandBuffer || !dstBuffer || !commandBuffer->wgpu_encoder) {
		return;
	}

	(void)dstOffset;
	(void)dataSize;
}

void vkCmdCopyQueryPoolResults(VkCommandBuffer commandBuffer, void *queryPool, uint32_t firstQuery,
                               uint32_t queryCount, VkBuffer dstBuffer, VkDeviceSize dstOffset,
                               VkDeviceSize stride, VkFlags flags) {
	(void)queryPool;
	(void)firstQuery;
	(void)queryCount;
	(void)dstBuffer;
	(void)dstOffset;
	(void)stride;
	(void)flags;

	if (!commandBuffer) {
		return;
	}
}
