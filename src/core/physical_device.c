#include "webvulkan_internal.h"

void vkGetPhysicalDeviceProperties(
    VkPhysicalDevice physicalDevice,
    void* pProperties)
{
    if (!physicalDevice || !pProperties) {
        return;
    }
    
    struct {
        uint32_t apiVersion;
        uint32_t driverVersion;
        uint32_t vendorID;
        uint32_t deviceID;
        uint32_t deviceType;
        char deviceName[256];
        uint8_t pipelineCacheUUID[16];
        uint32_t limits[52];
        uint32_t sparseProperties[5];
    }* props = pProperties;
    
    props->apiVersion = VK_API_VERSION_1_4;
    props->driverVersion = 1;
    props->vendorID = 0;
    props->deviceID = 0;
    props->deviceType = 1;
    memset(props->deviceName, 0, sizeof(props->deviceName));
    strncpy(props->deviceName, "WebGPU Device", sizeof(props->deviceName) - 1);
}

void vkGetPhysicalDeviceFeatures(
    VkPhysicalDevice physicalDevice,
    void* pFeatures)
{
    if (!physicalDevice || !pFeatures) {
        return;
    }
    
    uint32_t* features = pFeatures;
    memset(features, 0, 57 * sizeof(uint32_t));
    
    features[0] = 1;
    features[1] = 1;
    features[2] = 1;
    features[3] = 1;
    features[4] = 1;
    features[5] = 1;
    features[6] = 1;
    features[7] = 1;
    features[8] = 1;
    features[9] = 1;
}

void vkGetPhysicalDeviceMemoryProperties(
    VkPhysicalDevice physicalDevice,
    void* pMemoryProperties)
{
    if (!physicalDevice || !pMemoryProperties) {
        return;
    }
    
    struct {
        uint32_t memoryTypeCount;
        struct {
            uint32_t propertyFlags;
            uint32_t heapIndex;
        } memoryTypes[32];
        uint32_t memoryHeapCount;
        struct {
            VkDeviceSize size;
            uint32_t flags;
        } memoryHeaps[16];
    }* mem_props = pMemoryProperties;
    
    memset(mem_props, 0, sizeof(*mem_props));
    
    mem_props->memoryTypeCount = 1;
    mem_props->memoryTypes[0].propertyFlags = 0x7;
    mem_props->memoryTypes[0].heapIndex = 0;
    
    mem_props->memoryHeapCount = 1;
    mem_props->memoryHeaps[0].size = 256ULL * 1024 * 1024 * 1024;
    mem_props->memoryHeaps[0].flags = 0x1;
}

void vkGetPhysicalDeviceQueueFamilyProperties(
    VkPhysicalDevice physicalDevice,
    uint32_t* pQueueFamilyPropertyCount,
    void* pQueueFamilyProperties)
{
    if (!physicalDevice || !pQueueFamilyPropertyCount) {
        return;
    }
    
    if (!pQueueFamilyProperties) {
        *pQueueFamilyPropertyCount = 1;
        return;
    }
    
    struct {
        uint32_t queueFlags;
        uint32_t queueCount;
        uint32_t timestampValidBits;
        uint32_t minImageTransferGranularity[3];
    }* props = pQueueFamilyProperties;
    
    props[0].queueFlags = 0x7;
    props[0].queueCount = 1;
    props[0].timestampValidBits = 64;
    props[0].minImageTransferGranularity[0] = 1;
    props[0].minImageTransferGranularity[1] = 1;
    props[0].minImageTransferGranularity[2] = 1;
}

void vkGetPhysicalDeviceFormatProperties(
    VkPhysicalDevice physicalDevice,
    uint32_t format,
    void* pFormatProperties)
{
    (void)physicalDevice;
    (void)format;
    
    if (!pFormatProperties) {
        return;
    }
    
    uint32_t* props = pFormatProperties;
    props[0] = 0x1F;
    props[1] = 0x1F;
    props[2] = 0x1F;
}
