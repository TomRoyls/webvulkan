cmake_minimum_required(VERSION 3.20)

if(WEBVULKAN_WASM)
    add_executable(triangle triangle/triangle.c)
    target_include_directories(triangle PRIVATE 
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/external/Vulkan-Headers/include
    )
    target_link_libraries(triangle webvulkan)
endif()
