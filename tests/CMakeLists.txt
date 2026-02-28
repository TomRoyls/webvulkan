cmake_minimum_required(VERSION 3.20)

enable_testing()

add_executable(test_spirv test_spirv.c)
target_include_directories(test_spirv PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(test_spirv PRIVATE webvulkan_native)
target_compile_options(test_spirv PRIVATE -Wall -Wextra)
if(WEBVULKAN_SANITIZE)
    target_compile_options(test_spirv PRIVATE -fsanitize=address,undefined)
    target_link_options(test_spirv PUBLIC -fsanitize=address,undefined)
endif()
add_test(NAME test_spirv COMMAND test_spirv)
