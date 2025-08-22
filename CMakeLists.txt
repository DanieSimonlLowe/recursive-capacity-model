cmake_minimum_required(VERSION 3.10)
project(EVLifeCycle)

cmake_policy(SET CMP0079 NEW)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)
include_directories(/usr/include/eigen3)

# Add BayesOpt include + lib paths
include_directories(/bayesopt/include)
link_directories(/bayesopt/lib)

add_subdirectory(src)
add_subdirectory(tests)

add_executable(EVLifeCycle_main src/main.cpp)
target_link_libraries(EVLifeCycle_main EVLifeCycle_lib)
target_link_libraries(EVLifeCycle_main pthread)
target_link_libraries(EVLifeCycle_main bayesopt nlopt nfft3)  

target_link_libraries(EVLifeCycle_test nfft3)  

target_link_libraries(EVLifeCycle_test EVLifeCycle_lib gtest gmock gtest_main gmock_main)


