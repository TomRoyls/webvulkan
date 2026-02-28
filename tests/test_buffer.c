#include <assert.h>
#include <stdio.h>
#include <vulkan/vulkan.h>

static void test_create_null_device(void) {
	VkBufferCreateInfo info = {0};
	VkBuffer buffer = NULL;
	VkResult result = vkCreateBuffer(NULL, &info, NULL, &buffer);
	assert(result == VK_ERROR_INITIALIZATION_FAILED);
	assert(buffer == NULL);
	printf("[PASS] test_create_null_device\n");
}

static void test_create_null_create_info(void) {
	VkDevice fake_device = (VkDevice)(void *)1;
	VkBuffer buffer = NULL;
	VkResult result = vkCreateBuffer(fake_device, NULL, NULL, &buffer);
	assert(result == VK_ERROR_INITIALIZATION_FAILED);
	assert(buffer == NULL);
	printf("[PASS] test_create_null_create_info\n");
}

static void test_create_null_out_ptr(void) {
	VkDevice fake_device = (VkDevice)(void *)1;
	VkBufferCreateInfo info = {0};
	VkResult result = vkCreateBuffer(fake_device, &info, NULL, NULL);
	assert(result == VK_ERROR_INITIALIZATION_FAILED);
	printf("[PASS] test_create_null_out_ptr\n");
}

static void test_destroy_null(void) {
	vkDestroyBuffer(NULL, NULL, NULL);
	printf("[PASS] test_destroy_null\n");
}

int main(void) {
	test_create_null_device();
	test_create_null_create_info();
	test_create_null_out_ptr();
	test_destroy_null();
	printf("test_buffer: ALL PASSED\n");
	return 0;
}
