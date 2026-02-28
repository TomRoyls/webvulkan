#ifndef WEBVULKAN_H_
#define WEBVULKAN_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WEBVULKAN_VERSION_MAJOR 0
#define WEBVULKAN_VERSION_MINOR 1
#define WEBVULKAN_VERSION_PATCH 0

#define WEBVULKAN_MAKE_VERSION(major, minor, patch) \
    (((major) << 22) | ((minor) << 12) | (patch))

#define WEBVULKAN_VERSION \
    WEBVULKAN_MAKE_VERSION(WEBVULKAN_VERSION_MAJOR, WEBVULKAN_VERSION_MINOR, WEBVULKAN_VERSION_PATCH)

typedef enum WgvkResult {
    WGVK_SUCCESS = 0,
    WGVK_ERROR_UNKNOWN = -1,
    WGVK_ERROR_INVALID_SHADER = -2,
    WGVK_ERROR_OUT_OF_MEMORY = -3,
} WgvkResult;

typedef struct WgvkShaderCompileInfo {
    const uint32_t* spirvCode;
    size_t spirvSize;
    uint32_t stage;
    const char* entryPoint;
} WgvkShaderCompileInfo;

typedef struct WgvkShaderModule WgvkShaderModule;

WgvkResult wgvkCompileShaders(
    const WgvkShaderCompileInfo* pCompileInfos,
    uint32_t compileInfoCount,
    char** ppWgslOutputs);

void wgvkFreeWgslOutput(char* pWgslOutput);

uint32_t wgvkGetVersion(void);
const char* wgvkGetVersionString(void);

#ifdef __cplusplus
}
#endif

#endif
