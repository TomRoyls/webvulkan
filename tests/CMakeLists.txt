cmake_minimum_required(VERSION 3.20)

enable_testing()

add_executable(test_spirv test_spirv.c
    ${CMAKE_SOURCE_DIR}/src/shaders/spirv_parser.c
    ${CMAKE_SOURCE_DIR}/src/shaders/wgsl_gen.c
)
target_include_directories(test_spirv PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(NAME test_spirv COMMAND test_spirv)
