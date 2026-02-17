cmake_minimum_required(VERSION 3.20)

add_executable(test_instance test_instance.c)
target_link_libraries(test_instance webvulkan)

add_executable(test_device test_device.c)
target_link_libraries(test_device webvulkan)

add_executable(test_buffer test_buffer.c)
target_link_libraries(test_buffer webvulkan)

add_executable(test_image test_image.c)
target_link_libraries(test_image webvulkan)

add_executable(test_pipeline test_pipeline.c)
target_link_libraries(test_pipeline webvulkan)
