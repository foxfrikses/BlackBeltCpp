cmake_minimum_required(VERSION 3.21)
project(Nucleotide)

set(CMAKE_CXX_STANDARD 20)

add_executable(Nucleotide main.cpp nucleotide.h test_runner.h)
