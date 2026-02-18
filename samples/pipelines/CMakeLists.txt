cmake_minimum_required(VERSION 3.20)

add_executable(pipelines main.c)

target_link_libraries(pipelines PRIVATE webvulkan)

if(EMSCRIPTEN)
    set_target_properties(pipelines PROPERTIES
        SUFFIX ".html"
    )
    target_link_options(pipelines PRIVATE
        -sUSE_WEBGPU=1
        -sALLOW_MEMORY_GROWTH=1
        --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/../triangle/shell.html
    )
endif()
