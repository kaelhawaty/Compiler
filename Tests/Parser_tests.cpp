//
// Created by Mahmoudmohamed on 5/1/2021.
//

#include "gtest/gtest.h"
#include <vector>
#include <string>
#include <bits/stdc++.h>

#include "../src/Parser/Parser.h"

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
        std::vector<RegularExpression> regs = Parser::parse(R"(..\..\Tests\Input_samples\lab_input)");
        std::reverse(regs.begin(), regs.end());
        std::vector<std::string> testCases{"int", "rkgmnkrgn", "if", "while", "(", ";", "515151515", ">", "*"};
        std::vector<std::string> matched{"int", "id", "if", "while", "(", ";", "num", "relop", "mulop"};
        for (int i = 0; i < testCases.size(); i++) {
            EXPECT_EQ(tryAllRegExp(regs, testCases[i]), matched[i]);
        }
    }
}