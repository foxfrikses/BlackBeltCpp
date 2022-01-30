cmake_minimum_required(VERSION 3.21)
project(JsonPrinter)

set(CMAKE_CXX_STANDARD 20)

add_executable(JsonPrinter json_printer.cpp test_runner.h)
