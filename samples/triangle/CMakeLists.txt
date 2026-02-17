cmake_minimum_required(VERSION 3.20)

add_executable(triangle
    main.c
)

target_link_libraries(triangle
    PRIVATE webvulkan
)

if(EMSCRIPTEN)
    set_target_properties(triangle PROPERTIES
        SUFFIX ".html"
        HTML_FILE "${CMAKE_CURRENT_SOURCE_DIR}/shell.html"
    )
    target_link_options(triangle PRIVATE
        -sUSE_WEBGPU=1
        -sALLOW_MEMORY_GROWTH=1
        --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/shell.html
        --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/shaders@/shaders
    )
endif()

set(SHADER_SOURCES
    shaders/triangle.vert
    shaders/triangle.frag
)

foreach(SHADER IN LISTS SHADER_SOURCES)
    get_filename_component(SHADER_NAME ${SHADER} NAME_WE)
    get_filename_component(SHADER_EXT ${SHADER} EXT)
    
    if(SHADER_EXT STREQUAL ".vert")
        set(STAGE "--vertex")
    elseif(SHADER_EXT STREQUAL ".frag")
        set(STAGE "--fragment")
    elseif(SHADER_EXT STREQUAL ".comp")
        set(STAGE "--compute")
    endif()
    
    set(OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME}.spv")
    
    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        COMMAND glslangValidator -V ${STAGE} ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER} -o ${OUTPUT_FILE}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER}
        COMMENT "Compiling ${SHADER} to SPIR-V"
    )
    
    list(APPEND SPIRV_FILES ${OUTPUT_FILE})
endforeach()

add_custom_target(triangle_shaders ALL DEPENDS ${SPIRV_FILES})
add_dependencies(triangle triangle_shaders)
