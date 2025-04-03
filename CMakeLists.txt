cmake_minimum_required(VERSION 3.29.3)

project(my_alloc)

add_compile_options(-Wall -Wextra -Wpedantic)

add_link_options(-Wall -Wextra -Wpedantic)

set(shared_lib ${CMAKE_PROJECT_NAME}_shared)
set(static_lib ${CMAKE_PROJECT_NAME}_static)

add_library(${static_lib} STATIC)
add_library(${shared_lib} SHARED)

target_include_directories(${static_lib} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(${shared_lib} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_subdirectory(src)

enable_testing()

add_subdirectory(tests)

include(CTest)
