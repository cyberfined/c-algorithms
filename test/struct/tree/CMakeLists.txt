cmake_minimum_required(VERSION 3.14)

project(algorithms-struct-tree-test)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g")

add_executable(rbtree_test rbtree_test.c)
target_link_libraries(rbtree_test algorithms)

add_test(rbtree_test "./rbtree_test")
