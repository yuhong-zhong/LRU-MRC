cmake_minimum_required(VERSION 3.17)
project(lru_mrc)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-O3")

add_executable(lru_mrc main.cpp avl_tree.h)
