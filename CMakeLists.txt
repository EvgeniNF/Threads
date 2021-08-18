cmake_minimum_required(VERSION 3.20)
project(threads)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-static")

add_executable(threads main.cpp)
