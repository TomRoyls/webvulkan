cmake_minimum_required(VERSION 3.20)

enable_testing()

function(add_native_test name)
    add_executable(${name} ${CMAKE_CURRENT_SOURCE_DIR}/${name}.c)
    target_include_directories(${name} PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
    )
    target_link_libraries(${name} PRIVATE webvulkan_native)
    target_compile_options(${name} PRIVATE -Wall -Wextra)
    if(WEBVULKAN_SANITIZE)
        target_compile_options(${name} PRIVATE -fsanitize=address,undefined)
        target_link_options(${name} PUBLIC -fsanitize=address,undefined)
    endif()
    add_test(NAME ${name} COMMAND ${name})
endfunction()

# Tests exercising null-guard / error-path branches in the full
# Vulkan implementation. Compiled with webgpu_stubs.c so no real GPU needed.
function(add_objects_test name)
    add_executable(${name}
        ${CMAKE_CURRENT_SOURCE_DIR}/${name}.c
        ${CMAKE_SOURCE_DIR}/src/core/instance.c
        ${CMAKE_SOURCE_DIR}/src/core/device.c
        ${CMAKE_SOURCE_DIR}/src/core/physical_device.c
        ${CMAKE_SOURCE_DIR}/src/core/queue.c
        ${CMAKE_SOURCE_DIR}/src/objects/buffer.c
        ${CMAKE_SOURCE_DIR}/src/objects/image.c
        ${CMAKE_SOURCE_DIR}/src/objects/pipeline.c
        ${CMAKE_SOURCE_DIR}/src/objects/pipeline_layout.c
        ${CMAKE_SOURCE_DIR}/src/objects/shader_module.c
        ${CMAKE_SOURCE_DIR}/src/objects/descriptor_set.c
        ${CMAKE_SOURCE_DIR}/src/objects/descriptor_set_layout.c
        ${CMAKE_SOURCE_DIR}/src/objects/sampler.c
        ${CMAKE_SOURCE_DIR}/src/objects/image_view.c
        ${CMAKE_SOURCE_DIR}/src/objects/render_pass.c
        ${CMAKE_SOURCE_DIR}/src/objects/framebuffer.c
        ${CMAKE_SOURCE_DIR}/src/objects/command_pool.c
        ${CMAKE_SOURCE_DIR}/src/objects/command_buffer.c
        ${CMAKE_SOURCE_DIR}/src/objects/semaphore.c
        ${CMAKE_SOURCE_DIR}/src/objects/fence.c
        ${CMAKE_SOURCE_DIR}/src/objects/event.c
        ${CMAKE_SOURCE_DIR}/src/commands/draw.c
        ${CMAKE_SOURCE_DIR}/src/commands/compute.c
        ${CMAKE_SOURCE_DIR}/src/commands/copy.c
        ${CMAKE_SOURCE_DIR}/src/commands/sync.c
        ${CMAKE_SOURCE_DIR}/src/commands/render_pass.c
        ${CMAKE_SOURCE_DIR}/src/commands/subpass.c
        ${CMAKE_SOURCE_DIR}/src/sync/barrier.c
        ${CMAKE_SOURCE_DIR}/src/sync/push_constants.c
        ${CMAKE_SOURCE_DIR}/src/memory/device_memory.c
        ${CMAKE_SOURCE_DIR}/src/util/list.c
        ${CMAKE_SOURCE_DIR}/src/util/hash_table.c
        ${CMAKE_SOURCE_DIR}/src/util/log.c
        ${CMAKE_SOURCE_DIR}/src/shaders/spirv_parser.c
        ${CMAKE_SOURCE_DIR}/src/shaders/wgsl_gen.c
        ${CMAKE_CURRENT_SOURCE_DIR}/webgpu_stubs.c
    )
    target_include_directories(${name} PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
    )
    target_compile_options(${name} PRIVATE -Wall -Wextra)
    if(WEBVULKAN_SANITIZE)
        target_compile_options(${name} PRIVATE -fsanitize=address,undefined)
        target_link_options(${name} PUBLIC -fsanitize=address,undefined)
    endif()
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_native_test(test_spirv)

add_objects_test(test_instance)
add_objects_test(test_buffer)
add_objects_test(test_device)
add_objects_test(test_image)
add_objects_test(test_pipeline)
add_objects_test(test_lifecycle)
