cmake_minimum_required(VERSION 3.14)

project(algorithms-src)

add_library(algorithms "struct/pqueue/bheap.c" "struct/tree/rbtree.c" "struct/tree/avltree.c" "sort/hsort.c")
