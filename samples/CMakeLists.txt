cmake_minimum_required(VERSION 3.20)

if(WEBVULKAN_WASM)
    add_executable(triangle triangle.c)
    target_link_libraries(triangle webvulkan)
    target_include_directories(triangle PRIVATE ${CMAKE_SOURCE_DIR}/include)
    
    set_target_properties(triangle PROPERTIES
        SUFFIX ".js"
    )
    
    set(EMSCRIPTEN_LINKER_FLAGS "--use-port=emdawnwebgpu -sALLOW_MEMORY_GROWTH=1 -sASYNCIFY=1")
    target_link_options(triangle PRIVATE ${EMSCRIPTEN_LINKER_FLAGS})
endif()
