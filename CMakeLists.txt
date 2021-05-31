cmake_minimum_required(VERSION 3.15.3)
project(Compiler)

add_subdirectory(Tests)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS  "-static")

add_executable(Compiler main.cpp src/NFA/NFA.cpp src/NFA/NFA.h src/NFA/NFA_Builder.cpp src/NFA/NFA_Builder.h src/Parser/InputParser.cpp src/Parser/InputParser.h src/NFA/NFA.h src/Parser/ComponentParser.cpp src/Parser/ComponentParser.h src/Parser/RegularExpression.h src/Parser/Component.h src/Parser/Utils/ParserUtils.cpp src/Parser/Utils/ParserUtils.h src/Parser/LexicalParser.cpp src/Parser/LexicalParser.h src/DFA/DFA.cpp src/DFA/DFA.h src/Syntax_Parser/Rules_builder.cpp src/Syntax_Parser/Rules_builder.h src/Syntax_Parser/Syntax_Analyzer.h)