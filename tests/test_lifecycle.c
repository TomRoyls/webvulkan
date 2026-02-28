#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <vulkan/vulkan.h>
#include "webvulkan_internal.h"

static void test_instance_create_destroy(void) {
	VkApplicationInfo app_info = {
	    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
	    .pApplicationName = "lifecycle-test",
	    .apiVersion = VK_API_VERSION_1_0,
	};
	VkInstanceCreateInfo info = {
	    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
	    .pApplicationInfo = &app_info,
	};
	VkInstance instance = NULL;
	VkResult r = vkCreateInstance(&info, NULL, &instance);
	assert(r == VK_SUCCESS);
	assert(instance != NULL);
	vkDestroyInstance(instance, NULL);
	printf("[PASS] test_instance_create_destroy\n");
}

static void test_physical_device_enumerate(void) {
	VkInstanceCreateInfo info = {.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
	VkInstance instance = NULL;
	assert(vkCreateInstance(&info, NULL, &instance) == VK_SUCCESS);

	uint32_t count = 0;
	assert(vkEnumeratePhysicalDevices(instance, &count, NULL) == VK_SUCCESS);
	assert(count >= 1);

	VkPhysicalDevice phys_dev = NULL;
	count = 1;
	assert(vkEnumeratePhysicalDevices(instance, &count, &phys_dev) == VK_SUCCESS);
	assert(phys_dev != NULL);

	vkDestroyInstance(instance, NULL);
	printf("[PASS] test_physical_device_enumerate\n");
}

static void test_device_create_destroy(void) {
	VkInstanceCreateInfo info = {.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
	VkInstance instance = NULL;
	assert(vkCreateInstance(&info, NULL, &instance) == VK_SUCCESS);

	uint32_t count = 1;
	VkPhysicalDevice phys_dev = NULL;
	assert(vkEnumeratePhysicalDevices(instance, &count, &phys_dev) == VK_SUCCESS);

	phys_dev->wgpu_adapter = (WGPUAdapter)(uintptr_t)1;

	VkDeviceCreateInfo dev_info = {.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
	VkDevice device = NULL;
	VkResult r = vkCreateDevice(phys_dev, &dev_info, NULL, &device);
	assert(r == VK_SUCCESS);
	assert(device != NULL);

	vkDestroyDevice(device, NULL);
	vkDestroyInstance(instance, NULL);
	printf("[PASS] test_device_create_destroy\n");
}

static void test_buffer_create_destroy(void) {
	VkInstanceCreateInfo info = {.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
	VkInstance instance = NULL;
	assert(vkCreateInstance(&info, NULL, &instance) == VK_SUCCESS);

	uint32_t count = 1;
	VkPhysicalDevice phys_dev = NULL;
	assert(vkEnumeratePhysicalDevices(instance, &count, &phys_dev) == VK_SUCCESS);
	phys_dev->wgpu_adapter = (WGPUAdapter)(uintptr_t)1;

	VkDeviceCreateInfo dev_info = {.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
	VkDevice device = NULL;
	assert(vkCreateDevice(phys_dev, &dev_info, NULL, &device) == VK_SUCCESS);

	VkBufferCreateInfo buf_info = {
	    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
	    .size = 256,
	    .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
	};
	VkBuffer buffer = NULL;
	VkResult r = vkCreateBuffer(device, &buf_info, NULL, &buffer);
	assert(r == VK_SUCCESS);
	assert(buffer != NULL);

	vkDestroyBuffer(device, buffer, NULL);
	vkDestroyDevice(device, NULL);
	vkDestroyInstance(instance, NULL);
	printf("[PASS] test_buffer_create_destroy\n");
}

static void test_shader_module_wgsl(void) {
	VkInstanceCreateInfo info = {.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
	VkInstance instance = NULL;
	assert(vkCreateInstance(&info, NULL, &instance) == VK_SUCCESS);

	uint32_t count = 1;
	VkPhysicalDevice phys_dev = NULL;
	assert(vkEnumeratePhysicalDevices(instance, &count, &phys_dev) == VK_SUCCESS);
	phys_dev->wgpu_adapter = (WGPUAdapter)(uintptr_t)1;

	VkDeviceCreateInfo dev_info = {.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
	VkDevice device = NULL;
	assert(vkCreateDevice(phys_dev, &dev_info, NULL, &device) == VK_SUCCESS);

	const char *wgsl = "@vertex\n"
	                   "fn main() -> @builtin(position) vec4<f32> {\n"
	                   "    return vec4<f32>(0.0, 0.0, 0.0, 1.0);\n"
	                   "}\n";

	VkShaderModuleCreateInfo shader_info = {
	    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
	    .codeSize = __builtin_strlen(wgsl),
	    .pCode = (const uint32_t *)wgsl,
	};
	VkShaderModule shader = NULL;
	VkResult r = vkCreateShaderModule(device, &shader_info, NULL, &shader);
	assert(r == VK_SUCCESS);
	assert(shader != NULL);

	vkDestroyShaderModule(device, shader, NULL);
	vkDestroyDevice(device, NULL);
	vkDestroyInstance(instance, NULL);
	printf("[PASS] test_shader_module_wgsl\n");
}

static void test_pipeline_layout_create_destroy(void) {
	VkInstanceCreateInfo info = {.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
	VkInstance instance = NULL;
	assert(vkCreateInstance(&info, NULL, &instance) == VK_SUCCESS);

	uint32_t count = 1;
	VkPhysicalDevice phys_dev = NULL;
	assert(vkEnumeratePhysicalDevices(instance, &count, &phys_dev) == VK_SUCCESS);
	phys_dev->wgpu_adapter = (WGPUAdapter)(uintptr_t)1;

	VkDeviceCreateInfo dev_info = {.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
	VkDevice device = NULL;
	assert(vkCreateDevice(phys_dev, &dev_info, NULL, &device) == VK_SUCCESS);

	VkPipelineLayoutCreateInfo layout_info = {
	    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
	};
	VkPipelineLayout layout = NULL;
	VkResult r = vkCreatePipelineLayout(device, &layout_info, NULL, &layout);
	assert(r == VK_SUCCESS);
	assert(layout != NULL);

	vkDestroyPipelineLayout(device, layout, NULL);
	vkDestroyDevice(device, NULL);
	vkDestroyInstance(instance, NULL);
	printf("[PASS] test_pipeline_layout_create_destroy\n");
}

int main(void) {
	test_instance_create_destroy();
	test_physical_device_enumerate();
	test_device_create_destroy();
	test_buffer_create_destroy();
	test_shader_module_wgsl();
	test_pipeline_layout_create_destroy();
	printf("test_lifecycle: ALL PASSED\n");
	return 0;
}
