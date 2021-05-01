//
// Created by Abd Elkader on 5/1/2021.
//
#include "gtest/gtest.h"
#include "../src/Parser/InputParser.h"

namespace InputParser_tests{

    bool areEqual(const component& lhs, const component& rhs)
    {
        return lhs.type == rhs.type && lhs.regularDefinition == rhs.regularDefinition;
    }
    bool areEqual(const std::vector<component>& lhs, const std::vector<component>& rhs)
    {
        if(lhs.size() != rhs.size()) return false;
        for(int i=0 ; i< lhs.size() ;i++){
            if(!areEqual(lhs[i],rhs[i]))
                return false;
        }
        return true;
    }
    TEST(lab_input,lab_input){
        std::string inputFilePath = R"(..\..\Tests\Input_samples\lab_input)";
        InputParser inputParser = InputParser(inputFilePath);
        std::vector<std::pair<std::string, std::vector<component>>> regularDefinitionsComponents =
                                                             inputParser.getRegularDefinitionsComponents();
        std::vector<std::string> regularExpressions = inputParser.getRegularExpressions();

        std::vector<std::string> expectedRegExp
                {"id", "boolean", "int", "float", "num", "relop", "assign", "if", "else", "while",
                 ";", ",", "(", ")", "{", "}", "addop", "mulop"};

        EXPECT_TRUE(regularExpressions == expectedRegExp);

        std::unordered_map<std::string, std::vector<component>> expectedRegDef;
        //letter = a-z | A-Z
        expectedRegDef["letter"] = {component(RED_DEF,"a"),
                                    component(TO),
                                    component(RED_DEF,"z"),
                                    component(OR),
                                    component(RED_DEF,"A"),
                                    component(TO),
                                    component(RED_DEF,"Z")
        };
        //num: digit+ | digit+ . digits ( \L | E digits)
        expectedRegDef["num"] = {component(RED_DEF,"digit"),
                                 component(POS_CLOSURE),
                                 component(OR),
                                 component(RED_DEF,"digit"),
                                 component(POS_CLOSURE),
                                 component(CONCAT),
                                 component(RED_DEF,"."),
                                 component(CONCAT),
                                 component(RED_DEF,"digits"),
                                 component(CONCAT),
                                 component(OPEN_BRACKETS),
                                 component(RED_DEF,std::string(1,0)),
                                 component(OR),
                                 component(RED_DEF,"E"),
                                 component(CONCAT),
                                 component(RED_DEF,"digits"),
                                 component(CLOSE_BRACKETS)
        };
        for(std::pair<std::string, std::vector<component>>& pr : regularDefinitionsComponents){
            if(expectedRegDef.count(pr.first))
                EXPECT_TRUE(areEqual(pr.second, expectedRegDef[pr.first]));
        }
    }
}