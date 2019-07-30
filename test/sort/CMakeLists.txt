cmake_minimum_required(VERSION 3.14)

project(algorithms-sort-test)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g")

add_executable(hsort_test hsort_test.c)
target_link_libraries(hsort_test algorithms)

add_test(hsort_test "./hsort_test")
