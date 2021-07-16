cmake_minimum_required(VERSION 3.17)
project(sorted_stream_merge)

set(CMAKE_CXX_STANDARD 20)

add_executable(sorted_stream_merge main.cpp binary_heap.h)