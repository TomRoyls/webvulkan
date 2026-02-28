cmake_minimum_required(VERSION 3.20)
project(webvulkan VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(WEBVULKAN_BUILD_SAMPLES "Build sample applications" ON)
option(WEBVULKAN_BUILD_TESTS "Build tests" ON)
option(WEBVULKAN_WASM "Build for WebAssembly" OFF)
option(WEBVULKAN_SANITIZE "Enable AddressSanitizer + UBSan (non-WASM only)" OFF)

include(FetchContent)
FetchContent_Declare(
    vulkan_headers
    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git
    GIT_TAG v1.4.304
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(vulkan_headers)

set(VULKAN_HEADERS_DIR ${vulkan_headers_SOURCE_DIR}/include)

if(WEBVULKAN_WASM)
    if(NOT DEFINED EMSCRIPTEN)
        message(FATAL_ERROR "WEBVULKAN_WASM requires Emscripten toolchain")
    endif()
    set(CMAKE_EXECUTABLE_SUFFIX ".js")
    add_compile_options(--use-port=emdawnwebgpu)
    add_link_options(--use-port=emdawnwebgpu -sALLOW_MEMORY_GROWTH=1 -sENVIRONMENT=web)
endif()

if(WEBVULKAN_WASM)
    add_library(webvulkan
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
        src/commands/subpass.c
        src/shaders/spirv_parser.c
        src/shaders/wgsl_gen.c
        src/sync/barrier.c
        src/sync/push_constants.c
        src/memory/device_memory.c

        src/util/list.c
        src/util/hash_table.c
        src/util/log.c
    )

    target_include_directories(webvulkan
        PUBLIC
            ${VULKAN_HEADERS_DIR}
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

    target_compile_definitions(webvulkan PRIVATE
        VK_NO_PROTOTYPES
        VK_USE_PLATFORM_EMSCRIPTEN
    )

    target_compile_options(webvulkan PRIVATE
        -Wall -Wextra -Wpedantic
    )

    if(WEBVULKAN_BUILD_SAMPLES)
        add_subdirectory(samples)
    endif()
else()
    # Build a native library with the shader pipeline only (no WebGPU dependency)
    add_library(webvulkan_native STATIC
        src/shaders/spirv_parser.c
        src/shaders/wgsl_gen.c
    )
    target_include_directories(webvulkan_native
        PUBLIC
            ${VULKAN_HEADERS_DIR}
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
    target_compile_options(webvulkan_native PRIVATE
        -Wall -Wextra -Wpedantic
    )
    if(WEBVULKAN_SANITIZE)
        target_compile_options(webvulkan_native PRIVATE -fsanitize=address,undefined)
        target_link_options(webvulkan_native PUBLIC -fsanitize=address,undefined)
    endif()

    if(WEBVULKAN_BUILD_TESTS)
        add_subdirectory(tests)
    endif()
endif()

include(GNUInstallDirs)

install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/webvulkan.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/webgpu/webgpu.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(WEBVULKAN_WASM)
    install(TARGETS webvulkan
        EXPORT webvulkanTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    install(EXPORT webvulkanTargets
        FILE webvulkanTargets.cmake
        NAMESPACE webvulkan::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/webvulkan
    )

    include(CMakePackageConfigHelpers)
    configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/webvulkanConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/webvulkanConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/webvulkan
    )

    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/webvulkanConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/webvulkanConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/webvulkanConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/webvulkan
    )
endif()
