cmake_minimum_required(VERSION 3.20)

if(WEBVULKAN_WASM)
    add_executable(triangle triangle/triangle.c)
    target_include_directories(triangle PRIVATE ${CMAKE_SOURCE_DIR}/include)
endif()
