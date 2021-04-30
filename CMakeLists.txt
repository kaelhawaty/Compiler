cmake_minimum_required(VERSION 3.15.3)
project(Compiler)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(Tests)

add_executable(Compiler main.cpp src/NFAGenerator.cpp src/NFAGenerator.h src/NFA.h src/RegularDefinition.h src/ComponentParser.cpp src/ComponentParser.h)