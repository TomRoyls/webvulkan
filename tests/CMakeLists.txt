cmake_minimum_required(VERSION 3.20)

enable_testing()

add_executable(test_instance test_instance.c)
target_link_libraries(test_instance webvulkan)
add_test(NAME test_instance COMMAND test_instance)

add_executable(test_device test_device.c)
target_link_libraries(test_device webvulkan)
add_test(NAME test_device COMMAND test_device)

add_executable(test_buffer test_buffer.c)
target_link_libraries(test_buffer webvulkan)
add_test(NAME test_buffer COMMAND test_buffer)

add_executable(test_image test_image.c)
target_link_libraries(test_image webvulkan)
add_test(NAME test_image COMMAND test_image)

add_executable(test_pipeline test_pipeline.c)
target_link_libraries(test_pipeline webvulkan)
add_test(NAME test_pipeline COMMAND test_pipeline)
