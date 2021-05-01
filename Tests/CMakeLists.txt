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
add_executable(Tests NFA_tests.cpp ../src/NFA/NFA.cpp ../src/NFA/NFA.h ../src/NFA/NFA_Builder.cpp ../src/NFA/NFA_Builder.h ComponentParser_tests.cpp ../src/Parser/ComponentParser.cpp ../src/Parser/ComponentParser.h ../src/Parser/RegularExpression.h ../src/Parser/Component.h Parser_tests.cpp ../src/Parser/InputParser.h ../src/Parser/InputParser.cpp ../src/Parser/Parser.h ../src/Parser/Parser.cpp ../src/Parser/Utils/ParserUtils.h ../src/Parser/Utils/ParserUtils.cpp InputParser_tests.cpp)
target_link_libraries(Tests gtest_main)