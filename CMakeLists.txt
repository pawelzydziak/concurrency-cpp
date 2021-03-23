cmake_minimum_required(VERSION 3.17)
project(concurrency)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS -fgnu-tm)
add_executable(concurrency main.cpp)