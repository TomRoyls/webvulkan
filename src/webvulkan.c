#include "webvulkan.h"
#include <stdlib.h>
#include <string.h>
#include "shaders/spirv_parser.h"
#include "shaders/wgsl_gen.h"

static const char *const WGVK_VERSION_STRING = "0.1.0";

uint32_t wgvkGetVersion(void) {
	return WEBVULKAN_VERSION;
}

const char *wgvkGetVersionString(void) {
	return WGVK_VERSION_STRING;
}

WgvkResult wgvkCompileShaders(const WgvkShaderCompileInfo *pCompileInfos, uint32_t compileInfoCount,
                              char **ppWgslOutputs) {
	if (!pCompileInfos || !ppWgslOutputs || compileInfoCount == 0) {
		return WGVK_ERROR_UNKNOWN;
	}

	for (uint32_t i = 0; i < compileInfoCount; i++) {
		const WgvkShaderCompileInfo *info = &pCompileInfos[i];

		if (!info->spirvCode || info->spirvSize < 20) {
			ppWgslOutputs[i] = NULL;
			continue;
		}

		WgvkSpvModule *module = calloc(1, sizeof(WgvkSpvModule));
		if (!module) {
			ppWgslOutputs[i] = NULL;
			continue;
		}

		int parse_result = wgvk_spirv_parse(module, info->spirvCode, info->spirvSize / 4);
		if (parse_result != 0) {
			free(module);
			ppWgslOutputs[i] = NULL;
			continue;
		}

		WgvkWgslGenerator *gen = calloc(1, sizeof(WgvkWgslGenerator));
		if (!gen) {
			wgvk_spirv_free(module);
			free(module);
			ppWgslOutputs[i] = NULL;
			continue;
		}

		uint32_t exec_model = WGVK_SPV_EXEC_MODEL_VERTEX;
		if (info->stage == 1) {
			exec_model = WGVK_SPV_EXEC_MODEL_FRAGMENT;
		} else if (info->stage == 2) {
			exec_model = WGVK_SPV_EXEC_MODEL_GL_COMPUTE;
		}

		int init_result = wgvk_wgsl_init(gen, module, exec_model);
		if (init_result != 0) {
			free(gen);
			wgvk_spirv_free(module);
			free(module);
			ppWgslOutputs[i] = NULL;
			continue;
		}

		char *wgsl = wgvk_wgsl_generate(gen);
		ppWgslOutputs[i] = wgsl;

		wgvk_wgsl_free(gen);
		free(gen);
		wgvk_spirv_free(module);
		free(module);
	}

	return WGVK_SUCCESS;
}

void wgvkFreeWgslOutput(char *pWgslOutput) {
	free(pWgslOutput);
}
