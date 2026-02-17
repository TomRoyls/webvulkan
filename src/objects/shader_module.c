#include "webvulkan_internal.h"
#include <naga.h>

static void destroy_shader_module(void* obj) {
    VkShaderModule module = (VkShaderModule)obj;
    if (module->wgpu_shader) {
        wgpuShaderModuleRelease(module->wgpu_shader);
    }
    if (module->wgsl_source) {
        free(module->wgsl_source);
    }
    if (module->spirv_code) {
        free(module->spirv_code);
    }
    free(module);
}

static VkResult spirv_to_wgsl(
    const uint32_t* spirv_code,
    size_t spirv_size,
    char** wgsl_out)
{
    naga_module_t module = {0};
    naga_error_t error = naga_parse_spirv(spirv_code, spirv_size / 4, &module);
    if (error != NAGA_OK) {
        return VK_ERROR_INVALID_SHADER_NV;
    }
    
    char* wgsl = NULL;
    error = naga_emit_wgsl(&module, &wgsl);
    naga_module_free(&module);
    
    if (error != NAGA_OK || !wgsl) {
        return VK_ERROR_INVALID_SHADER_NV;
    }
    
    *wgsl_out = wgsl;
    return VK_SUCCESS;
}

VkResult vkCreateShaderModule(
    VkDevice device,
    const VkShaderModuleCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkShaderModule* pShaderModule)
{
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
        module->spirv_code = wgvk_alloc(pCreateInfo->codeSize);
        if (!module->spirv_code) {
            free(module);
            return VK_ERROR_OUT_OF_HOST_MEMORY;
        }
        memcpy(module->spirv_code, pCreateInfo->pCode, pCreateInfo->codeSize);
        module->spirv_size = pCreateInfo->codeSize;
        
        VkResult result = spirv_to_wgsl(
            pCreateInfo->pCode,
            pCreateInfo->codeSize,
            &module->wgsl_source);
        
        if (result != VK_SUCCESS) {
            free(module->spirv_code);
            free(module);
            return result;
        }
        
        WGPUShaderModuleWGSLDescriptor wgsl_desc = {
            .chain = {
                .sType = WGPUSType_ShaderSourceWGSL,
            },
            .code = module->wgsl_source,
        };
        
        WGPUShaderModuleDescriptor desc = {
            .nextInChain = (const WGPUChainedStruct*)&wgsl_desc,
        };
        
        module->wgpu_shader = wgpuDeviceCreateShaderModule(device->wgpu_device, &desc);
        if (!module->wgpu_shader) {
            free(module->wgsl_source);
            free(module->spirv_code);
            free(module);
            return VK_ERROR_OUT_OF_DEVICE_MEMORY;
        }
    }
    
    *pShaderModule = module;
    return VK_SUCCESS;
}

void vkDestroyShaderModule(
    VkDevice device,
    VkShaderModule shaderModule,
    const VkAllocationCallbacks* pAllocator)
{
    (void)device;
    (void)pAllocator;
    if (shaderModule) {
        wgvk_object_release(&shaderModule->base);
    }
}
