#include <assert.h>
#include <stdio.h>
#include <vulkan/vulkan.h>

static void test_create_null_physical_device(void) {
	VkDevice device = NULL;
	VkResult result = vkCreateDevice(NULL, NULL, NULL, &device);
	assert(result == VK_ERROR_INITIALIZATION_FAILED);
	assert(device == NULL);
	printf("[PASS] test_create_null_physical_device\n");
}

static void test_create_null_out_ptr(void) {
	VkPhysicalDevice fake = (VkPhysicalDevice)(void *)1;
	VkResult result = vkCreateDevice(fake, NULL, NULL, NULL);
	assert(result == VK_ERROR_INITIALIZATION_FAILED);
	printf("[PASS] test_create_null_out_ptr\n");
}

static void test_destroy_null(void) {
	vkDestroyDevice(NULL, NULL);
	printf("[PASS] test_destroy_null\n");
}

static void test_device_wait_idle_null(void) {
	VkResult result = vkDeviceWaitIdle(NULL);
	assert(result == VK_SUCCESS);
	printf("[PASS] test_device_wait_idle_null\n");
}

static void test_get_device_queue_null_device(void) {
	VkQueue queue = (VkQueue)(void *)0xDEAD;
	vkGetDeviceQueue(NULL, 0, 0, &queue);
	printf("[PASS] test_get_device_queue_null_device\n");
}

static void test_get_device_queue_null_out_ptr(void) {
	VkDevice fake = (VkDevice)(void *)1;
	vkGetDeviceQueue(fake, 0, 0, NULL);
	printf("[PASS] test_get_device_queue_null_out_ptr\n");
}

int main(void) {
	test_create_null_physical_device();
	test_create_null_out_ptr();
	test_destroy_null();
	test_device_wait_idle_null();
	test_get_device_queue_null_device();
	test_get_device_queue_null_out_ptr();
	printf("test_device: ALL PASSED\n");
	return 0;
}
