//
// Created by Karim on 6/2/2021.
//

#include "gtest/gtest.h"
#include <vector>
#include <string>
#include <bits/stdc++.h>

#include "../src/Parser/LexicalParser.h"
#include "../src/Syntax_Parser/Syntax_parser.h"
#include "Syntax_tests_helper.h"
#include "../src/Syntax_Parser/Rules_builder.h"

namespace Syntax_parser_tests {
    class SyntaxParserTest : public ::testing::Test {
    protected:
        std::fstream tempProgramFile;
        std::string tempProgramPath{::testing::TempDir() + "tempProgram.txt"};
        std::fstream tempCFGFile;
        std::string tempCFGPath{::testing::TempDir() + "tempCFG.txt"};

        void SetUp() override {
            tempProgramFile.open(tempProgramPath, std::fstream::in | std::fstream::out | std::fstream::trunc);
            ASSERT_TRUE(tempProgramFile.is_open());
            tempCFGFile.open(tempCFGPath, std::fstream::in | std::fstream::out | std::fstream::trunc);
            ASSERT_TRUE(tempCFGFile.is_open());
        }

        template<typename... Args>
        void writeProgram(Args &&... lines) {
            ((tempProgramFile << lines << "\n"), ...);
            tempProgramFile.flush();
        }

        template<typename... Args>
        void writeCFG(Args &&... lines) {
            ((tempCFGFile << lines << "\n"), ...);
            tempCFGFile.flush();
        }

        void TearDown() override {
            if (tempProgramFile.is_open()) {
                tempProgramFile.close();
            }
            if (tempCFGFile.is_open()) {
                tempCFGFile.close();
            }
            std::remove(tempProgramPath.c_str());
            std::remove(tempCFGPath.c_str());
        }
    };


    TEST_F(SyntaxParserTest, Declaration) {
        LexicalParser lexicalParser(R"(..\..\Tests\Input_samples\lab_input)");
        EXPECT_FALSE(lexicalParser.has_grammar_error());

        writeProgram("int x;");
        lexicalParser.set_input_stream(tempProgramPath);

        writeCFG("# DECLARATION = PRIMITIVE_TYPE 'id' ';'",
                 "# PRIMITIVE_TYPE = 'int' | 'float'");
        Rules_builder builder{tempCFGPath};

        Syntax_parser syn_parser{builder.getRules(), builder.getStartSymbol()};
        EXPECT_FALSE(syn_parser.fail());

        std::vector<std::vector<Symbol>> expected{
                writeSymbols({"DECLARATION"}),
                writeSymbols({"PRIMITIVE_TYPE", "'id'", "';'"}),
                writeSymbols({"'int'", "'id'", "';'"}),
        };

        auto[actual, status] = syn_parser.parse(lexicalParser);
        EXPECT_TRUE(status == Syntax_parser::Status::ACCEPTED);
        EXPECT_TRUE(expected == actual);
    }

    TEST_F(SyntaxParserTest, Assignment) {
        LexicalParser lexicalParser(R"(..\..\Tests\Input_samples\lab_input)");
        EXPECT_FALSE(lexicalParser.has_grammar_error());

        writeProgram("int x = 5;");
        lexicalParser.set_input_stream(tempProgramPath);

        writeCFG("# ASSIGNMENT = DECLARATION 'assign' 'num' ';'",
                 "# DECLARATION = PRIMITIVE_TYPE 'id'",
                 "# PRIMITIVE_TYPE = 'int' | 'float'");
        Rules_builder builder{tempCFGPath};

        Syntax_parser syn_parser{builder.getRules(), builder.getStartSymbol()};
        EXPECT_FALSE(syn_parser.fail());

        std::vector<std::vector<Symbol>> expected{
                writeSymbols({"ASSIGNMENT"}),
                writeSymbols({"DECLARATION", "'assign'", "'num'", "';'"}),
                writeSymbols({"PRIMITIVE_TYPE", "'id'", "'assign'", "'num'", "';'"}),
                writeSymbols({"'int'", "'id'", "'assign'", "'num'", "';'"}),
        };

        auto[actual, status] = syn_parser.parse(lexicalParser);
        EXPECT_TRUE(status == Syntax_parser::Status::ACCEPTED);
        EXPECT_TRUE(expected == actual);
    }

    TEST_F(SyntaxParserTest, WrongTerminalSymbol) {
        LexicalParser lexicalParser(R"(..\..\Tests\Input_samples\lab_input)");
        EXPECT_FALSE(lexicalParser.has_grammar_error());

        writeProgram("int x 5;");
        lexicalParser.set_input_stream(tempProgramPath);

        writeCFG("# ASSIGNMENT = DECLARATION 'assign' 'num' ';'",
                 "# DECLARATION = PRIMITIVE_TYPE 'id'",
                 "# PRIMITIVE_TYPE = 'int' | 'float'");
        Rules_builder builder{tempCFGPath};

        Syntax_parser syn_parser{builder.getRules(), builder.getStartSymbol()};
        EXPECT_FALSE(syn_parser.fail());

        std::vector<std::vector<Symbol>> expected{
                writeSymbols({"ASSIGNMENT"}),
                writeSymbols({"DECLARATION", "'assign'", "'num'", "';'"}),
                writeSymbols({"PRIMITIVE_TYPE", "'id'", "'assign'", "'num'", "';'"}),
                writeSymbols({"'int'", "'id'", "'assign'", "'num'", "';'"}),
        };

        auto[actual, status] = syn_parser.parse(lexicalParser);
        EXPECT_TRUE(status == Syntax_parser::Status::ACCEPTED_WITH_ERRORS);
        EXPECT_TRUE(expected == actual);
    }

    TEST_F(SyntaxParserTest, SyncEntry) {
        LexicalParser lexicalParser(R"(..\..\Tests\Input_samples\lab_input)");
        EXPECT_FALSE(lexicalParser.has_grammar_error());

        writeProgram("= = 5;");
        lexicalParser.set_input_stream(tempProgramPath);

        writeCFG("# ASSIGNMENT = 'assign' DECLARATION 'assign' 'num' ';'",
                 "# DECLARATION = PRIMITIVE_TYPE 'id'",
                 "# PRIMITIVE_TYPE = 'int' | 'float'");
        Rules_builder builder{tempCFGPath};

        Syntax_parser syn_parser{builder.getRules(), builder.getStartSymbol()};
        EXPECT_FALSE(syn_parser.fail());

        std::vector<std::vector<Symbol>> expected{
                writeSymbols({"ASSIGNMENT"}),
                writeSymbols({"'assign'", "DECLARATION", "'assign'", "'num'", "';'"}),
                writeSymbols({"'assign'", "'assign'", "'num'", "';'"}),
        };

        auto[actual, status] = syn_parser.parse(lexicalParser);
        EXPECT_TRUE(status == Syntax_parser::Status::ACCEPTED_WITH_ERRORS);
        EXPECT_TRUE(expected == actual);
    }

    TEST_F(SyntaxParserTest, EmptyEntry) {
        LexicalParser lexicalParser(R"(..\..\Tests\Input_samples\lab_input)");
        EXPECT_FALSE(lexicalParser.has_grammar_error());

        writeProgram("WrongWord int x = 5;");
        lexicalParser.set_input_stream(tempProgramPath);

        writeCFG("# ASSIGNMENT = DECLARATION 'assign' 'num' ';'",
                 "# DECLARATION = PRIMITIVE_TYPE 'id'",
                 "# PRIMITIVE_TYPE = 'int' | 'float'");
        Rules_builder builder{tempCFGPath};

        Syntax_parser syn_parser{builder.getRules(), builder.getStartSymbol()};
        EXPECT_FALSE(syn_parser.fail());

        std::vector<std::vector<Symbol>> expected{
                writeSymbols({"ASSIGNMENT"}),
                writeSymbols({"DECLARATION", "'assign'", "'num'", "';'"}),
                writeSymbols({"PRIMITIVE_TYPE", "'id'", "'assign'", "'num'", "';'"}),
                writeSymbols({"'int'", "'id'", "'assign'", "'num'", "';'"}),
        };

        auto[actual, status] = syn_parser.parse(lexicalParser);
        EXPECT_TRUE(status == Syntax_parser::Status::ACCEPTED_WITH_ERRORS);
        EXPECT_TRUE(expected == actual);
    }

    TEST_F(SyntaxParserTest, TableConstructionError) {
        // No left hand side
        writeCFG("# A = 'b' 'c' | 'b' 'd'");
        Rules_builder builder{tempCFGPath};

        Syntax_parser syn_parser{builder.getRules(), builder.getStartSymbol()};
        EXPECT_TRUE(syn_parser.fail());
    }

}