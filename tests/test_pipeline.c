#include <assert.h>
#include <stdio.h>
#include <vulkan/vulkan.h>

static void test_create_graphics_null_device(void) {
	VkGraphicsPipelineCreateInfo info = {0};
	VkPipeline pipeline = NULL;
	VkResult result = vkCreateGraphicsPipelines(NULL, VK_NULL_HANDLE, 1, &info, NULL, &pipeline);
	assert(result == VK_ERROR_INITIALIZATION_FAILED);
	assert(pipeline == NULL);
	printf("[PASS] test_create_graphics_null_device\n");
}

static void test_create_graphics_null_create_infos(void) {
	VkDevice fake_device = (VkDevice)(void *)1;
	VkPipeline pipeline = NULL;
	VkResult result =
	    vkCreateGraphicsPipelines(fake_device, VK_NULL_HANDLE, 1, NULL, NULL, &pipeline);
	assert(result == VK_ERROR_INITIALIZATION_FAILED);
	assert(pipeline == NULL);
	printf("[PASS] test_create_graphics_null_create_infos\n");
}

static void test_create_graphics_null_out_ptr(void) {
	VkDevice fake_device = (VkDevice)(void *)1;
	VkGraphicsPipelineCreateInfo info = {0};
	VkResult result = vkCreateGraphicsPipelines(fake_device, VK_NULL_HANDLE, 1, &info, NULL, NULL);
	assert(result == VK_ERROR_INITIALIZATION_FAILED);
	printf("[PASS] test_create_graphics_null_out_ptr\n");
}

static void test_create_compute_null_device(void) {
	VkComputePipelineCreateInfo info = {0};
	VkPipeline pipeline = NULL;
	VkResult result = vkCreateComputePipelines(NULL, VK_NULL_HANDLE, 1, &info, NULL, &pipeline);
	assert(result == VK_ERROR_INITIALIZATION_FAILED);
	assert(pipeline == NULL);
	printf("[PASS] test_create_compute_null_device\n");
}

static void test_create_compute_null_create_infos(void) {
	VkDevice fake_device = (VkDevice)(void *)1;
	VkPipeline pipeline = NULL;
	VkResult result =
	    vkCreateComputePipelines(fake_device, VK_NULL_HANDLE, 1, NULL, NULL, &pipeline);
	assert(result == VK_ERROR_INITIALIZATION_FAILED);
	assert(pipeline == NULL);
	printf("[PASS] test_create_compute_null_create_infos\n");
}

static void test_destroy_null(void) {
	vkDestroyPipeline(NULL, NULL, NULL);
	printf("[PASS] test_destroy_null\n");
}

int main(void) {
	test_create_graphics_null_device();
	test_create_graphics_null_create_infos();
	test_create_graphics_null_out_ptr();
	test_create_compute_null_device();
	test_create_compute_null_create_infos();
	test_destroy_null();
	printf("test_pipeline: ALL PASSED\n");
	return 0;
}
