cmake_minimum_required(VERSION 3.10)
# tests/CMakeLists.txt

# Find or fetch GoogleTest
set(BINARY ${CMAKE_PROJECT_NAME}_test)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)

set(SOURCES ${TEST_SOURCES})

add_executable(${BINARY} ${TEST_SOURCES})

add_test(NAME ${BINARY} COMMAND ${BINARY})

target_link_libraries(EVLifeCycle_test EVLifeCycle_lib gtest gmock gtest_main gmock_main)