/*
 * test_instance.c — Native unit tests for VkInstance null-guard / error paths.
 *
 * These tests exercise only the argument-validation branches that execute
 * before any WebGPU call, so they run on the native (non-WASM) build without
 * a GPU present.
 */
#include <assert.h>
#include <stdio.h>
#include <vulkan/vulkan.h>

static void test_create_null_create_info(void) {
	VkInstance instance = NULL;
	VkResult result = vkCreateInstance(NULL, NULL, &instance);
	assert(result == VK_ERROR_INITIALIZATION_FAILED);
	assert(instance == NULL);
	printf("[PASS] test_create_null_create_info\n");
}

static void test_create_null_out_ptr(void) {
	VkInstanceCreateInfo info = {0};
	VkResult result = vkCreateInstance(&info, NULL, NULL);
	assert(result == VK_ERROR_INITIALIZATION_FAILED);
	printf("[PASS] test_create_null_out_ptr\n");
}

static void test_destroy_null(void) {
	vkDestroyInstance(NULL, NULL);
	printf("[PASS] test_destroy_null\n");
}

static void test_enumerate_null_instance(void) {
	uint32_t count = 0;
	VkResult result = vkEnumeratePhysicalDevices(NULL, &count, NULL);
	assert(result == VK_ERROR_INITIALIZATION_FAILED);
	printf("[PASS] test_enumerate_null_instance\n");
}

static void test_enumerate_null_count(void) {
	/* Construct a fake non-NULL pointer — we only need to pass the NULL check on
	 * pInstance, not actually use it.  We read pPhysicalDeviceCount before
	 * touching the instance struct, so the test is safe. */
	VkInstance fake = (VkInstance)(void *)1;
	VkResult result = vkEnumeratePhysicalDevices(fake, NULL, NULL);
	assert(result == VK_ERROR_INITIALIZATION_FAILED);
	printf("[PASS] test_enumerate_null_count\n");
}

int main(void) {
	test_create_null_create_info();
	test_create_null_out_ptr();
	test_destroy_null();
	test_enumerate_null_instance();
	test_enumerate_null_count();
	printf("test_instance: ALL PASSED\n");
	return 0;
}
