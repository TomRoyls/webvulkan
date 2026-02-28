#include <stdlib.h>
#include <string.h>
#include "../shaders/spirv_parser.h"
#include "../shaders/wgsl_gen.h"
#include "../util/log.h"
#include "webvulkan_internal.h"

static void destroy_shader_module(void *obj) {
	VkShaderModule module = (VkShaderModule)obj;
	if (module->wgpu_shader) {
		wgpuShaderModuleRelease(module->wgpu_shader);
	}
	if (module->wgsl_source) {
		wgvk_free(module->wgsl_source);
	}
	if (module->spirv_code) {
		wgvk_free(module->spirv_code);
	}
	wgvk_free(module);
}

static VkBool32 is_wgsl_source(const uint32_t *code, size_t size) {
	if (size < 4)
		return VK_FALSE;
	return (code[0] == 0x07230203) ? VK_FALSE : VK_TRUE;
}

VkResult vkCreateShaderModule(VkDevice device, const VkShaderModuleCreateInfo *pCreateInfo,
                              const VkAllocationCallbacks *pAllocator,
                              VkShaderModule *pShaderModule) {
	(void)pAllocator;

	if (!device || !pCreateInfo || !pShaderModule) {
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	VkShaderModule module = wgvk_alloc(sizeof(struct VkShaderModule_T));
	if (!module) {
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	wgvk_object_init(&module->base, destroy_shader_module);
	module->device = device;
	module->wgpu_shader = NULL;
	module->wgsl_source = NULL;
	module->spirv_code = NULL;
	module->spirv_size = 0;

	if (pCreateInfo->codeSize > 0 && pCreateInfo->pCode) {
		if (is_wgsl_source(pCreateInfo->pCode, pCreateInfo->codeSize)) {
			size_t wgsl_len = pCreateInfo->codeSize;
			module->wgsl_source = wgvk_alloc(wgsl_len + 1);
			if (!module->wgsl_source) {
				wgvk_free(module);
				return VK_ERROR_OUT_OF_HOST_MEMORY;
			}
			memcpy(module->wgsl_source, pCreateInfo->pCode, wgsl_len);
			module->wgsl_source[wgsl_len] = '\0';
		} else {
			module->spirv_code = wgvk_alloc(pCreateInfo->codeSize);
			if (!module->spirv_code) {
				wgvk_free(module);
				return VK_ERROR_OUT_OF_HOST_MEMORY;
			}
			memcpy(module->spirv_code, pCreateInfo->pCode, pCreateInfo->codeSize);
			module->spirv_size = pCreateInfo->codeSize;

			WgvkSpvModule spv_module = {0};
			if (wgvk_spirv_parse(&spv_module, pCreateInfo->pCode, pCreateInfo->codeSize / 4) == 0) {
				WgvkWgslGenerator gen = {0};
				uint32_t exec_model = WGVK_SPV_EXEC_MODEL_VERTEX;
				for (uint32_t i = 0; i < spv_module.entry_point_count; i++) {
					exec_model = spv_module.entry_points[i].exec_model;
					break;
				}
				if (wgvk_wgsl_init(&gen, &spv_module, exec_model) == 0) {
					module->wgsl_source = wgvk_wgsl_generate(&gen);
					wgvk_wgsl_free(&gen);
				}
				wgvk_spirv_free(&spv_module);
			}

			if (!module->wgsl_source) {
				/* SPIR-V transpilation produced no WGSL output. Rather than silently
				 * substituting a stub shader that would produce incorrect rendering,
				 * report the failure so the caller can handle it. */
				WGVK_ERROR(WGVK_LOG_CAT_SHADER,
				           "SPIR-V to WGSL transpilation failed: no WGSL output generated. "
				           "Consider passing WGSL source directly to vkCreateShaderModule.");
				wgvk_free(module->spirv_code);
				wgvk_free(module);
				return VK_ERROR_INVALID_SHADER_NV;
			}

			WGPUShaderSourceWGSL wgsl_desc = {
			    .chain =
			        {
			            .next = NULL,
			            .sType = WGPUSType_ShaderSourceWGSL,
			        },
			    .code = (WGPUStringView){.data = module->wgsl_source, .length = WGPU_STRLEN},
			};

			WGPUShaderModuleDescriptor desc = {
			    .nextInChain = (const WGPUChainedStruct *)&wgsl_desc,
			};

			module->wgpu_shader = wgpuDeviceCreateShaderModule(device->wgpu_device, &desc);
			if (!module->wgpu_shader) {
				wgvk_free(module->wgsl_source);
				wgvk_free(module->spirv_code);
				wgvk_free(module);
				return VK_ERROR_OUT_OF_DEVICE_MEMORY;
			}
		}

		*pShaderModule = module;
		return VK_SUCCESS;
	}

	*pShaderModule = module;
	return VK_SUCCESS;
}

void vkDestroyShaderModule(VkDevice device, VkShaderModule shaderModule,
                           const VkAllocationCallbacks *pAllocator) {
	(void)device;
	(void)pAllocator;
	if (shaderModule) {
		wgvk_object_release(&shaderModule->base);
	}
}
