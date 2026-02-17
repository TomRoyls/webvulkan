#include "webvulkan_internal.h"

void vkCmdPipelineBarrier(
    VkCommandBuffer commandBuffer,
    uint32_t srcStageMask,
    uint32_t dstStageMask,
    VkDependencyFlags dependencyFlags,
    uint32_t memoryBarrierCount,
    const void* pMemoryBarriers,
    uint32_t bufferMemoryBarrierCount,
    const void* pBufferMemoryBarriers,
    uint32_t imageMemoryBarrierCount,
    const void* pImageMemoryBarriers)
{
    (void)srcStageMask;
    (void)dstStageMask;
    (void)dependencyFlags;
    (void)memoryBarrierCount;
    (void)pMemoryBarriers;
    (void)bufferMemoryBarrierCount;
    (void)pBufferMemoryBarriers;
    (void)imageMemoryBarrierCount;
    (void)pImageMemoryBarriers;
    
    if (!commandBuffer) {
        return;
    }
}

void vkCmdMemoryBarrier(
    VkCommandBuffer commandBuffer,
    uint32_t srcStageMask,
    uint32_t dstStageMask,
    const void* pMemoryBarrier)
{
    (void)srcStageMask;
    (void)dstStageMask;
    (void)pMemoryBarrier;
    
    if (!commandBuffer) {
        return;
    }
}

void vkCmdBufferBarrier(
    VkCommandBuffer commandBuffer,
    uint32_t srcStageMask,
    uint32_t dstStageMask,
    const void* pBufferMemoryBarrier)
{
    (void)srcStageMask;
    (void)dstStageMask;
    (void)pBufferMemoryBarrier;
    
    if (!commandBuffer) {
        return;
    }
}

void vkCmdImageBarrier(
    VkCommandBuffer commandBuffer,
    uint32_t srcStageMask,
    uint32_t dstStageMask,
    const void* pImageMemoryBarrier)
{
    (void)srcStageMask;
    (void)dstStageMask;
    (void)pImageMemoryBarrier;
    
    if (!commandBuffer) {
        return;
    }
}

void vkCmdSetEvent(
    VkCommandBuffer commandBuffer,
    VkEvent event,
    uint32_t stageMask)
{
    (void)stageMask;
    
    if (!commandBuffer || !event) {
        return;
    }
    
    event->signaled = true;
}

void vkCmdResetEvent(
    VkCommandBuffer commandBuffer,
    VkEvent event,
    uint32_t stageMask)
{
    (void)stageMask;
    
    if (!commandBuffer || !event) {
        return;
    }
    
    event->signaled = false;
}

void vkCmdWaitEvents(
    VkCommandBuffer commandBuffer,
    uint32_t eventCount,
    const VkEvent* pEvents,
    uint32_t srcStageMask,
    uint32_t dstStageMask,
    uint32_t memoryBarrierCount,
    const void* pMemoryBarriers,
    uint32_t bufferMemoryBarrierCount,
    const void* pBufferMemoryBarriers,
    uint32_t imageMemoryBarrierCount,
    const void* pImageMemoryBarriers)
{
    (void)srcStageMask;
    (void)dstStageMask;
    (void)memoryBarrierCount;
    (void)pMemoryBarriers;
    (void)bufferMemoryBarrierCount;
    (void)pBufferMemoryBarriers;
    (void)imageMemoryBarrierCount;
    (void)pImageMemoryBarriers;
    
    if (!commandBuffer || !pEvents) {
        return;
    }
    
    (void)eventCount;
}
