cmake_minimum_required(VERSION 3.14)
project(cyc_strong_kar)

set(CMAKE_CXX_STANDARD 14)

include_directories(../include/)
include_directories(src)

set()

add_executable(cyc_strong_kar
    src/plugin.cpp
    src/plugin.hpp)
