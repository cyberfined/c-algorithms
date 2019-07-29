cmake_minimum_required(VERSION 3.14)

project(algorithms-struct-pqueue-test)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast")

add_executable(bheap_test bheap_test.c)
target_link_libraries(bheap_test algorithms)

add_test(bheap_test "./bheap_test")
