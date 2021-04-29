cmake_minimum_required(VERSION 3.16)
project(Compiler)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(Tests)

add_executable(Compiler main.cpp src/NFA.cpp src/NFA.h src/NFA_Builder.cpp src/NFA_Builder.h)