//
// Created by Abd Elkader on 5/1/2021.
//
#include "gtest/gtest.h"
#include "../src/Parser/InputParser.h"

namespace InputParser_tests{

    TEST(lab_input,lab_input){
        std::string inputFilePath = R"(..\..\Tests\Input_samples\lab_input)";
        InputParser inputParser = InputParser(inputFilePath);
        std::vector<std::pair<std::string, std::vector<component>>> regularDefinitionsComponents =
                                                             inputParser.getRegularDefinitionsComponents();
        std::vector<std::string> regularExpressions = inputParser.getRegularExpressions();

        std::vector<std::string> expectedRegExp{"boolean", "int", "float", "if", "else", "while", ";", ",", "(", ")",
                                                "{", "}", "id", "num", "relop", "assign", "addop", "mulop"};

        EXPECT_TRUE(regularExpressions == expectedRegExp);

        std::unordered_map<std::string, std::vector<component>> expectedRegDef;
        //letter = a-z | A-Z
        expectedRegDef["letter"] = {{RED_DEF, "a"},
                                    {TO, ""},
                                    {RED_DEF, "z"},
                                    {OR, ""},
                                    {RED_DEF, "A"},
                                    {TO, ""},
                                    {RED_DEF, "Z"}
        };
        //num: digit+ | digit+ . digits ( \L | E digits)
        expectedRegDef["num"] = {{RED_DEF, "digit"},
                                 {POS_CLOSURE, ""},
                                 {OR, ""},
                                 {RED_DEF, "digit"},
                                 {POS_CLOSURE, ""},
                                 {CONCAT, ""},
                                 {RED_DEF, "."},
                                 {CONCAT, ""},
                                 {RED_DEF, "digits"},
                                 {CONCAT, ""},
                                 {OPEN_BRACKETS, ""},
                                 {RED_DEF, std::string{0}},
                                 {OR, ""},
                                 {RED_DEF, "E"},
                                 {CONCAT, ""},
                                 {RED_DEF, "digits"},
                                 {CLOSE_BRACKETS, ""}
        };
        for(const auto& pr : regularDefinitionsComponents){
            if(expectedRegDef.count(pr.first))
                EXPECT_TRUE(pr.second ==  expectedRegDef[pr.first]);
        }
    }
}