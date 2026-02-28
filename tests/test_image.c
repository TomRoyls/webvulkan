#include <assert.h>
#include <stdio.h>
#include <vulkan/vulkan.h>

static void test_create_null_device(void) {
	VkImageCreateInfo info = {0};
	VkImage image = NULL;
	VkResult result = vkCreateImage(NULL, &info, NULL, &image);
	assert(result == VK_ERROR_INITIALIZATION_FAILED);
	assert(image == NULL);
	printf("[PASS] test_create_null_device\n");
}

static void test_create_null_create_info(void) {
	VkDevice fake_device = (VkDevice)(void *)1;
	VkImage image = NULL;
	VkResult result = vkCreateImage(fake_device, NULL, NULL, &image);
	assert(result == VK_ERROR_INITIALIZATION_FAILED);
	assert(image == NULL);
	printf("[PASS] test_create_null_create_info\n");
}

static void test_create_null_out_ptr(void) {
	VkDevice fake_device = (VkDevice)(void *)1;
	VkImageCreateInfo info = {0};
	VkResult result = vkCreateImage(fake_device, &info, NULL, NULL);
	assert(result == VK_ERROR_INITIALIZATION_FAILED);
	printf("[PASS] test_create_null_out_ptr\n");
}

static void test_destroy_null(void) {
	vkDestroyImage(NULL, NULL, NULL);
	printf("[PASS] test_destroy_null\n");
}

int main(void) {
	test_create_null_device();
	test_create_null_create_info();
	test_create_null_out_ptr();
	test_destroy_null();
	printf("test_image: ALL PASSED\n");
	return 0;
}
