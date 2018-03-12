cmake_minimum_required(VERSION 3.9)
project(MI_PDP_semestral)

set(CMAKE_CXX_STANDARD 11)
SET(GCC_COVERAGE_COMPILE_FLAGS "-O3")
SET(GCC_COVERAGE_LINK_FLAGS    "-O3")

add_executable(MI_PDP_semestral
        main.cpp)
