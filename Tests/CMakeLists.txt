include(FetchContent)
FetchContent_Declare(
        googletest
        # Specify the commit you depend on and update it regularly.
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(Tests
        NFA_tests.cpp
        ../src/NFA/NFA.cpp
        ../src/NFA/NFA.h
        ../src/NFA/NFA_Builder.cpp
        ../src/NFA/NFA_Builder.h
        ComponentParser_tests.cpp
        ../src/Parser/ComponentParser.cpp
        ../src/Parser/ComponentParser.h
        ../src/Parser/RegularExpression.h
        ../src/Parser/Component.h
        LexicalParser_tests.cpp
        ../src/Parser/InputParser.h
        ../src/Parser/InputParser.cpp
        ../src/Parser/LexicalParser.h
        ../src/Parser/LexicalParser.cpp
        ../src/Parser/Utils/ParserUtils.h
        ../src/Parser/Utils/ParserUtils.cpp
        InputParser_tests.cpp
        ../src/DFA/DFA.h
        ../src/DFA/DFA.cpp
        DFA_tests.cpp
        ../src/Syntax_Parser/Syntax_Utils.h
        ../src/Syntax_Parser/Syntax_Utils.cpp
        Syntax_Utils_tests.cpp
        ../src/Syntax_Parser/ParsingTable.h
        ../src/Syntax_Parser/ParsingTable.cpp
        ParsingTable_test.cpp
        Syntax_tests_helper.h
        Syntax_parser_test.cpp
        ../src/Syntax_Parser/Syntax_parser.cpp
        ../src/Syntax_Parser/Syntax_parser.h
        ../src/Syntax_Parser/Rules_builder.cpp
        ../src/Syntax_Parser/Rules_builder.h
        Rules_builder_tests.cpp)
target_link_libraries(Tests gtest_main)