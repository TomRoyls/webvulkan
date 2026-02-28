# WebVulkan API Coverage

## Legend

- ✅ Fully implemented
- 🟡 Partially implemented
- 🔴 Not implemented
- ⚪ Not applicable to WebGPU

## Core Functions

### Instance

| Function | Status | Notes |
|----------|--------|-------|
| `vkCreateInstance` | ✅ | Creates WGPUInstance |
| `vkDestroyInstance` | ✅ | |
| `vkEnumerateInstanceExtensionProperties` | 🔴 | |
| `vkEnumerateInstanceLayerProperties` | 🔴 | |
| `vkEnumerateInstanceVersion` | 🔴 | |

### Physical Device

| Function | Status | Notes |
|----------|--------|-------|
| `vkEnumeratePhysicalDevices` | ✅ | Returns single cached device |
| `vkGetPhysicalDeviceProperties` | ✅ | |
| `vkGetPhysicalDeviceFeatures` | ✅ | |
| `vkGetPhysicalDeviceMemoryProperties` | ✅ | |
| `vkGetPhysicalDeviceQueueFamilyProperties` | ✅ | |
| `vkGetPhysicalDeviceFormatProperties` | 🟡 | Basic formats only |
| `vkGetPhysicalDeviceImageFormatProperties` | 🔴 | |

### Device

| Function | Status | Notes |
|----------|--------|-------|
| `vkCreateDevice` | ✅ | |
| `vkDestroyDevice` | ✅ | |
| `vkGetDeviceQueue` | ✅ | |
| `vkDeviceWaitIdle` | ✅ | |

## Resource Objects

### Buffers

| Function | Status | Notes |
|----------|--------|-------|
| `vkCreateBuffer` | ✅ | |
| `vkDestroyBuffer` | ✅ | |
| `vkGetBufferMemoryRequirements` | ✅ | |
| `vkBindBufferMemory` | ✅ | |

### Images

| Function | Status | Notes |
|----------|--------|-------|
| `vkCreateImage` | ✅ | 2D images |
| `vkDestroyImage` | ✅ | |
| `vkGetImageMemoryRequirements` | ✅ | |
| `vkBindImageMemory` | ✅ | |
| `vkGetImageSubresourceLayout` | 🔴 | |

### Image Views

| Function | Status | Notes |
|----------|--------|-------|
| `vkCreateImageView` | ✅ | |
| `vkDestroyImageView` | ✅ | |

### Samplers

| Function | Status | Notes |
|----------|--------|-------|
| `vkCreateSampler` | ✅ | |
| `vkDestroySampler` | ✅ | |

### Memory

| Function | Status | Notes |
|----------|--------|-------|
| `vkAllocateMemory` | ✅ | Host-visible only |
| `vkFreeMemory` | ✅ | |
| `vkMapMemory` | ✅ | |
| `vkUnmapMemory` | ✅ | |
| `vkFlushMappedMemoryRanges` | 🟡 | No-op (coherent) |
| `vkInvalidateMappedMemoryRanges` | 🟡 | No-op (coherent) |

## Pipeline Objects

### Shaders

| Function | Status | Notes |
|----------|--------|-------|
| `vkCreateShaderModule` | ✅ | SPIR-V or direct WGSL |
| `vkDestroyShaderModule` | ✅ | |

### Pipelines

| Function | Status | Notes |
|----------|--------|-------|
| `vkCreateGraphicsPipelines` | ✅ | |
| `vkCreateComputePipelines` | ✅ | |
| `vkDestroyPipeline` | ✅ | |

### Pipeline Layout

| Function | Status | Notes |
|----------|--------|-------|
| `vkCreatePipelineLayout` | ✅ | |
| `vkDestroyPipelineLayout` | ✅ | |

### Descriptors

| Function | Status | Notes |
|----------|--------|-------|
| `vkCreateDescriptorSetLayout` | ✅ | |
| `vkDestroyDescriptorSetLayout` | ✅ | |
| `vkCreateDescriptorPool` | ✅ | |
| `vkDestroyDescriptorPool` | ✅ | |
| `vkAllocateDescriptorSets` | ✅ | |
| `vkFreeDescriptorSets` | ✅ | |
| `vkUpdateDescriptorSets` | ✅ | |

### Render Pass

| Function | Status | Notes |
|----------|--------|-------|
| `vkCreateRenderPass` | ✅ | |
| `vkDestroyRenderPass` | ✅ | |
| `vkCreateRenderPass2` | 🔴 | |

### Framebuffer

| Function | Status | Notes |
|----------|--------|-------|
| `vkCreateFramebuffer` | ✅ | |
| `vkDestroyFramebuffer` | ✅ | |

## Command Buffers

| Function | Status | Notes |
|----------|--------|-------|
| `vkCreateCommandPool` | ✅ | |
| `vkDestroyCommandPool` | ✅ | |
| `vkAllocateCommandBuffers` | ✅ | |
| `vkFreeCommandBuffers` | ✅ | |
| `vkBeginCommandBuffer` | ✅ | |
| `vkEndCommandBuffer` | ✅ | |
| `vkResetCommandBuffer` | 🟡 | |
| `vkResetCommandPool` | 🟡 | |

## Drawing Commands

| Function | Status | Notes |
|----------|--------|-------|
| `vkCmdDraw` | ✅ | |
| `vkCmdDrawIndexed` | ✅ | |
| `vkCmdDrawInstanced` | ✅ | |
| `vkCmdDrawIndexedInstanced` | ✅ | |
| `vkCmdBindVertexBuffers` | ✅ | |
| `vkCmdBindIndexBuffer` | ✅ | |
| `vkCmdBindPipeline` | ✅ | |
| `vkCmdBindDescriptorSets` | ✅ | |

## Compute Commands

| Function | Status | Notes |
|----------|--------|-------|
| `vkCmdDispatch` | ✅ | |
| `vkCmdDispatchIndirect` | 🔴 | |

## Render Pass Commands

| Function | Status | Notes |
|----------|--------|-------|
| `vkCmdBeginRenderPass` | ✅ | |
| `vkCmdEndRenderPass` | ✅ | |
| `vkCmdNextSubpass` | 🟡 | |
| `vkCmdSetViewport` | 🟡 | Stored, not applied |
| `vkCmdSetScissor` | 🟡 | Stored, not applied |
| `vkCmdSetLineWidth` | ⚪ | WebGPU doesn't support |
| `vkCmdSetDepthBias` | ⚪ | |
| `vkCmdSetBlendConstants` | ⚪ | |

## Transfer Commands

| Function | Status | Notes |
|----------|--------|-------|
| `vkCmdCopyBuffer` | ✅ | |
| `vkCmdCopyImage` | 🟡 | |
| `vkCmdCopyBufferToImage` | ✅ | |
| `vkCmdCopyImageToBuffer` | ✅ | |
| `vkCmdBlitImage` | 🔴 | |
| `vkCmdResolveImage` | 🔴 | |
| `vkCmdClearColorImage` | 🔴 | |
| `vkCmdClearDepthStencilImage` | 🔴 | |

## Synchronization

### Fences

| Function | Status | Notes |
|----------|--------|-------|
| `vkCreateFence` | ✅ | |
| `vkDestroyFence` | ✅ | |
| `vkResetFences` | ✅ | |
| `vkGetFenceStatus` | ✅ | |
| `vkWaitForFences` | ✅ | |

### Semaphores

| Function | Status | Notes |
|----------|--------|-------|
| `vkCreateSemaphore` | ✅ | Binary only |
| `vkDestroySemaphore` | ✅ | |
| `vkGetSemaphoreCounterValue` | 🔴 | Timeline not supported |
| `vkWaitSemaphores` | 🔴 | |
| `vkSignalSemaphore` | 🔴 | |

### Events

| Function | Status | Notes |
|----------|--------|-------|
| `vkCreateEvent` | ✅ | |
| `vkDestroyEvent` | ✅ | |
| `vkGetEventStatus` | ✅ | |
| `vkSetEvent` | ✅ | |
| `vkResetEvent` | ✅ | |
| `vkCmdSetEvent` | 🟡 | |
| `vkCmdResetEvent` | 🟡 | |
| `vkCmdWaitEvents` | 🟡 | |

### Barriers

| Function | Status | Notes |
|----------|--------|-------|
| `vkCmdPipelineBarrier` | ✅ | Basic support |

## Queue Submission

| Function | Status | Notes |
|----------|--------|-------|
| `vkQueueSubmit` | ✅ | |
| `vkQueueWaitIdle` | ✅ | |

## Push Constants

| Function | Status | Notes |
|----------|--------|-------|
| `vkCmdPushConstants` | ✅ | Via uniform buffer |

## Query Operations

| Function | Status | Notes |
|----------|--------|-------|
| `vkCreateQueryPool` | 🔴 | |
| `vkDestroyQueryPool` | 🔴 | |
| `vkGetQueryPoolResults` | 🔴 | |
| `vkCmdResetQueryPool` | 🔴 | |
| `vkCmdBeginQuery` | 🔴 | |
| `vkCmdEndQuery` | 🔴 | |
| `vkCmdWriteTimestamp` | 🔴 | |
| `vkCmdCopyQueryPoolResults` | 🔴 | |

## Summary Statistics

| Category | Implemented | Partial | Not Implemented |
|----------|-------------|---------|-----------------|
| Core | 8 | 1 | 3 |
| Resources | 16 | 2 | 1 |
| Pipeline | 15 | 0 | 1 |
| Commands | 12 | 4 | 2 |
| Compute | 1 | 0 | 1 |
| Render Pass | 4 | 2 | 0 |
| Transfer | 3 | 1 | 4 |
| Sync | 13 | 3 | 4 |
| Queries | 0 | 0 | 8 |
| **Total** | **72** | **13** | **24** |

**Coverage: ~66%** of core Vulkan 1.0 API
