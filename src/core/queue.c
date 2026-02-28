#include "webvulkan_internal.h"

VkResult vkQueueSubmit(VkQueue queue, uint32_t submitCount, const VkSubmitInfo *pSubmits,
                       VkFence fence) {
	if (!queue) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	for (uint32_t i = 0; i < submitCount; i++) {
		const VkSubmitInfo *submit = &pSubmits[i];

		WGPUCommandBuffer *cmd_buffers = NULL;
		if (submit->commandBufferCount > 0) {
			cmd_buffers = wgvk_alloc(submit->commandBufferCount * sizeof(WGPUCommandBuffer));
			if (!cmd_buffers) {
				return VK_ERROR_OUT_OF_HOST_MEMORY;
			}

			for (uint32_t j = 0; j < submit->commandBufferCount; j++) {
				VkCommandBuffer cmd = submit->pCommandBuffers[j];
				if (cmd && cmd->wgpu_encoder) {
					WGPUCommandBufferDescriptor desc = {};
					cmd_buffers[j] = wgpuCommandEncoderFinish(cmd->wgpu_encoder, &desc);
				} else {
					cmd_buffers[j] = NULL;
				}
			}
		}

		if (cmd_buffers && submit->commandBufferCount > 0) {
			wgpuQueueSubmit(queue->wgpu_queue, submit->commandBufferCount, cmd_buffers);

			for (uint32_t j = 0; j < submit->commandBufferCount; j++) {
				if (cmd_buffers[j]) {
					wgpuCommandBufferRelease(cmd_buffers[j]);
				}
			}
			free(cmd_buffers);
		}
	}

	if (fence) {
		fence->signaled = VK_TRUE;
	}

	return VK_SUCCESS;
}

VkResult vkQueueWaitIdle(VkQueue queue) {
	if (!queue) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	wgpuQueueSubmit(queue->wgpu_queue, 0, NULL);

	return VK_SUCCESS;
}
