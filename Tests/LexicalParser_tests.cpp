//
// Created by Mahmoudmohamed on 5/1/2021.
//

#include "gtest/gtest.h"
#include <vector>
#include <string>
#include <bits/stdc++.h>

#include "../src/Parser/LexicalParser.h"

namespace Parser_tests {
    class LexicalParserTest : public ::testing::Test {
    protected:
        std::fstream tempRulesFile;
        std::string tempRulesPath{::testing::TempDir() + "tempRules.txt"};
        std::fstream tempProgramFile;
        std::string tempProgramPath{::testing::TempDir() + "tempProgram.txt"};

        void SetUp() override {
            tempRulesFile.open(tempRulesPath, std::fstream::in | std::fstream::out | std::fstream::trunc);
            ASSERT_TRUE(tempRulesFile.is_open());
            tempProgramFile.open(tempProgramPath, std::fstream::in | std::fstream::out | std::fstream::trunc);
            ASSERT_TRUE(tempProgramFile.is_open());
        }

        template<typename... Args>
        void writeRules(Args &&... lines) {
            ((tempRulesFile << lines << "\n"), ...);
            tempRulesFile.flush();
        }

        template<typename... Args>
        void writeProgram(Args &&... lines) {
            ((tempProgramFile << lines << "\n"), ...);
            tempProgramFile.flush();
        }

        void TearDown() override {
            if (tempRulesFile.is_open()) {
                tempRulesFile.close();
            }
            if (tempProgramFile.is_open()) {
                tempProgramFile.close();
            }
            std::remove(tempRulesPath.c_str());
            std::remove(tempProgramPath.c_str());
        }
    };

    TEST(AssignmentFileOutput, AssignmentFileOutput) {
        LexicalParser lexicalParser(R"(..\..\Tests\Input_samples\lab_input)");
        EXPECT_FALSE(lexicalParser.has_grammar_error());
        lexicalParser.set_input_stream(R"(..\..\Tests\Input_samples\program_input.txt)");
        std::vector<std::string> ExpectedTokens{"int", "id", ",", "id", ",", "id", ",", "id", ";", "while", "(", "id",
                                                "relop", "num", ")", "{", "id", "assign", "id", "addop", "num", ";", "}"};

        for (const auto &ExpectedToken : ExpectedTokens) {
            Token token;
            ASSERT_TRUE(lexicalParser.get_next_token(token));
            EXPECT_EQ(token.regEXP, ExpectedToken);
        }
    }

    TEST_F(LexicalParserTest, SimpleCase) {
        writeRules("s1 : Alice", "s2 : Bob");
        LexicalParser lexicalParser(tempRulesPath);
        EXPECT_FALSE(lexicalParser.has_grammar_error());
        writeProgram("Alice Bob");

        lexicalParser.set_input_stream(tempProgramPath);

        std::vector<Token> expectedTokens{{"s1", "Alice"},
                                          {"s2", "Bob"}};

        for (const auto &expected : expectedTokens) {
            Token token;
            ASSERT_TRUE(lexicalParser.get_next_token(token));
            EXPECT_EQ(token.regEXP, expected.regEXP);
            EXPECT_EQ(token.match_string, expected.match_string);
        }
    }

    TEST_F(LexicalParserTest, IgnoresFirstChar) {
        writeRules("s1 : Alice", "s2 : Bob");
        LexicalParser lexicalParser(tempRulesPath);
        EXPECT_FALSE(lexicalParser.has_grammar_error());
        writeProgram("AAlice BBob");

        lexicalParser.set_input_stream(tempProgramPath);

        std::vector<Token> expectedTokens{{"s1", "Alice"},
                                          {"s2", "Bob"}};

        for (const auto &expected : expectedTokens) {
            Token token;
            ASSERT_TRUE(lexicalParser.get_next_token(token));
            EXPECT_EQ(token.regEXP, expected.regEXP);
            EXPECT_EQ(token.match_string, expected.match_string);
        }
    }

    TEST_F(LexicalParserTest, IgnoresTwoChar) {
        writeRules("s1 : Alice", "s2 : Bob");
        LexicalParser lexicalParser(tempRulesPath);
        EXPECT_FALSE(lexicalParser.has_grammar_error());
        writeProgram("AAAlice BBBob");

        lexicalParser.set_input_stream(tempProgramPath);

        std::vector<Token> expectedTokens{{"s1", "Alice"},
                                          {"s2", "Bob"}};

        for (const auto &expected : expectedTokens) {
            Token token;
            ASSERT_TRUE(lexicalParser.get_next_token(token));
            EXPECT_EQ(token.regEXP, expected.regEXP);
            EXPECT_EQ(token.match_string, expected.match_string);
        }
    }

    TEST_F(LexicalParserTest, AcceptMinimalPriority) {
        writeRules("letter = a-z | A-Z", "letters : letter+", "TwoLetters : letter letter");
        LexicalParser lexicalParser(tempRulesPath);
        EXPECT_FALSE(lexicalParser.has_grammar_error());
        writeProgram("ab");

        lexicalParser.set_input_stream(tempProgramPath);

        std::vector<Token> expectedTokens{{"letters", "ab"}};

        for (const auto &expected : expectedTokens) {
            Token token;
            ASSERT_TRUE(lexicalParser.get_next_token(token));
            EXPECT_EQ(token.regEXP, expected.regEXP);
            EXPECT_EQ(token.match_string, expected.match_string);
        }
    }

    TEST_F(LexicalParserTest, MaximalMunch) {
        writeRules("letter = a-z | A-Z", "digit = 0-9", "letters : letter+", "letterdigit : letter+ digit");
        LexicalParser lexicalParser(tempRulesPath);
        EXPECT_FALSE(lexicalParser.has_grammar_error());
        writeProgram("HelloWorld5");

        lexicalParser.set_input_stream(tempProgramPath);

        std::vector<Token> expectedTokens{{"letterdigit", "HelloWorld5"}};

        for (const auto &expected : expectedTokens) {
            Token token;
            ASSERT_TRUE(lexicalParser.get_next_token(token));
            EXPECT_EQ(token.regEXP, expected.regEXP);
            EXPECT_EQ(token.match_string, expected.match_string);
        }
    }

    TEST_F(LexicalParserTest, NoMatch) {
        writeRules("letter = a-z | A-Z", "digit = 0-9", "letters : letter+", "letterdigit : letter+ digit");
        LexicalParser lexicalParser(tempRulesPath);
        EXPECT_FALSE(lexicalParser.has_grammar_error());
        writeProgram("2387#@");

        lexicalParser.set_input_stream(tempProgramPath);

        Token token;
        EXPECT_FALSE(lexicalParser.get_next_token(token));
    }

    TEST_F(LexicalParserTest, HbeedChecker) {
        writeRules("letter = a-z | A-Z", "Habeed : Hazem", "Word : letter+", "Punctuation : \\:", "Random: Word+ \\@\\#");
        LexicalParser lexicalParser(tempRulesPath);
        EXPECT_FALSE(lexicalParser.has_grammar_error());
        writeProgram("Hazem says :", "sha@#");

        lexicalParser.set_input_stream(tempProgramPath);

        std::vector<Token> expectedTokens{
                {"Habeed",      "Hazem"},
                {"Word",        "says"},
                {"Punctuation", ":"},
                {"Random",      "sha@#"}};

        for (const auto &expected : expectedTokens) {
            Token token;
            ASSERT_TRUE(lexicalParser.get_next_token(token));
            EXPECT_EQ(token.regEXP, expected.regEXP);
            EXPECT_EQ(token.match_string, expected.match_string);
        }
    }

    TEST_F(LexicalParserTest, GrammarCheck) {
        writeRules("letter = a-z | (A-Z", "digit = 0-9", "letters : letter+", "letterdigit : letter+ digit", "digits : digit+");
        LexicalParser lexicalParser(tempRulesPath);
        EXPECT_TRUE(lexicalParser.has_grammar_error());
        writeProgram("2387#@ 1515 mfkfmefm1551");

        lexicalParser.set_input_stream(tempProgramPath);

        std::vector<Token> expectedTokens{
                {"digits","2387"},
                {"digits","1515"},
                {"digits","1551"}};

        for (const auto &expected : expectedTokens) {
            Token token;
            ASSERT_TRUE(lexicalParser.get_next_token(token));
            EXPECT_EQ(token.regEXP, expected.regEXP);
            EXPECT_EQ(token.match_string, expected.match_string);
        }
    }

}