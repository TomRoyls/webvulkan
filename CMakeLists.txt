cmake_minimum_required(VERSION 3.20)
project(webvulkan VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(WEBVULKAN_BUILD_SAMPLES "Build sample applications" ON)
option(WEBVULKAN_BUILD_TESTS "Build tests" ON)
option(WEBVULKAN_WASM "Build for WebAssembly" OFF)

if(WEBVULKAN_WASM)
    if(NOT DEFINED EMSCRIPTEN)
        message(FATAL_ERROR "WEBVULKAN_WASM requires Emscripten toolchain")
    endif()
    set(USE_WEBGPU ON)
    add_compile_options(-sUSE_WEBGPU=1)
    add_link_options(-sUSE_WEBGPU=1 -sALLOW_MEMORY_GROWTH=1)
endif()

add_library(webvulkan SHARED
    src/core/instance.c
    src/core/device.c
    src/core/physical_device.c
    src/core/queue.c
    src/objects/buffer.c
    src/objects/image.c
    src/objects/image_view.c
    src/objects/render_pass.c
    src/objects/framebuffer.c
    src/objects/pipeline.c
    src/objects/pipeline_layout.c
    src/objects/shader_module.c
    src/objects/descriptor_set.c
    src/objects/descriptor_set_layout.c
    src/objects/sampler.c
    src/objects/command_pool.c
    src/objects/command_buffer.c
    src/objects/semaphore.c
    src/objects/fence.c
    src/objects/event.c
    src/commands/draw.c
    src/commands/compute.c
    src/commands/copy.c
    src/commands/sync.c
    src/commands/render_pass.c
    src/shaders/spirv.c
    src/shaders/wgsl.c
    src/shaders/naga.c
    src/sync/barrier.c
    src/memory/device_memory.c
    src/memory/allocator.c
    src/util/list.c
    src/util/hash_table.c
)

target_include_directories(webvulkan
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(webvulkan PRIVATE
    VK_NO_PROTOTYPES
    WEBVULKAN_IMPLEMENTATION
)

if(USE_WEBGPU)
    target_compile_definitions(webvulkan PRIVATE WEBVULKAN_USE_WEBGPU)
endif()

if(WEBVULKAN_BUILD_SAMPLES)
    add_subdirectory(samples)
endif()

if(WEBVULKAN_BUILD_TESTS AND NOT WEBVULKAN_WASM)
    add_subdirectory(tests)
endif()

install(TARGETS webvulkan
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/webvulkan
    DESTINATION include
)

install(FILES include/vulkan.h
    DESTINATION include
)
