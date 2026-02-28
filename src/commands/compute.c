#include "webvulkan_internal.h"

void vkCmdDispatch(VkCommandBuffer commandBuffer, uint32_t groupCountX, uint32_t groupCountY,
                   uint32_t groupCountZ) {
	if (!commandBuffer || !commandBuffer->wgpu_compute_pass) {
		return;
	}

	wgpuComputePassEncoderDispatchWorkgroups(commandBuffer->wgpu_compute_pass, groupCountX,
	                                         groupCountY, groupCountZ);
}

void vkCmdDispatchIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset) {
	if (!commandBuffer || !buffer || !commandBuffer->wgpu_compute_pass) {
		return;
	}

	wgpuComputePassEncoderDispatchWorkgroupsIndirect(commandBuffer->wgpu_compute_pass,
	                                                 buffer->wgpu_buffer, offset);
}
