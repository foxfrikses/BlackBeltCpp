cmake_minimum_required(VERSION 3.21)
project(SVG)

set(CMAKE_CXX_STANDARD 20)

add_executable(SVG test.cpp svg.h svg.cpp)
