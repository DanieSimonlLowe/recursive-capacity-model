cmake_minimum_required(VERSION 3.10)
project(EVLifeCycle)

cmake_policy(SET CMP0079 NEW)


set(CMAKE_CXX_STANDARD 14)

include_directories(src)
include_directories(/usr/include/eigen3)

add_subdirectory(src)
add_subdirectory(tests)


target_link_libraries(EVLifeCycle_test EVLifeCycle_lib gtest gmock gtest_main gmock_main) # Or   for modern CMake
