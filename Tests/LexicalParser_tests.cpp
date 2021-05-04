//
// Created by Mahmoudmohamed on 5/1/2021.
//

#include "gtest/gtest.h"
#include <vector>
#include <string>
#include <bits/stdc++.h>

#include "../src/Parser/LexicalParser.h"

namespace Parser_tests {

    bool MatchRegexp(const std::string &s, const NFA &nfa) {
        NFA::Set st{nfa.get_start()};
        st = E_closure(st);
        for (char c: s) {
            st = E_closure(Move(st, c));
        }
        return st.count(nfa.get_end()) != 0;
    }

    std::string tryAllRegExp(const std::vector<RegularExpression> &v, const std::string &target) {
        for (const RegularExpression &reg: v) {
            if (MatchRegexp(target, reg.getNFA())) {
                return reg.getName();
            }
        }
        // This shouldn't match to any of the test cases.
        return "";
    }


    TEST(ParsingFile, simpleRules) {
        LexicalParser lexicalParser(R"(..\..\Tests\Input_samples\lab_input)");
        lexicalParser.set_input_stream(R"(..\..\Tests\Input_samples\program_input.txt)");
        std::vector<std::string> ExpectedTokens{"int", "id", ",", "id", ",", "id", ",", "id", ";", "while", "(", "id",
                                                "relop", "num", ")", "{", "id", "assign", "id", "addop", "num", ";", "}"};

        for (const auto &ExpectedToken : ExpectedTokens) {
            Token token;
            EXPECT_TRUE(lexicalParser.get_next_token(token));
            EXPECT_EQ(token.regEXP, ExpectedToken);
        }
    }
}