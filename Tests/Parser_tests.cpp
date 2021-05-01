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

    bool tryAllRegExp(std::vector<RegularExpression>& v, const std::string& target) {
        for (RegularExpression reg: v) {
            if (MatchRegexp(target, reg.getNFA())) {
                std::cout << target << " " << reg.getName() << "\n";
                return true;
            }
        }
        return false;
    }


    TEST(ParsingFile, simpleRules) {
        Parser parser;
        std::vector<RegularExpression> regs = parser.parse(R"(F:\Compiler\Compiler\input)");
        std::reverse(regs.begin(), regs.end());
        EXPECT_TRUE(tryAllRegExp(regs, "int"));
        EXPECT_TRUE(tryAllRegExp(regs, "rkgmnkrgn"));
        EXPECT_TRUE(tryAllRegExp(regs, "if"));
        EXPECT_TRUE(tryAllRegExp(regs, "while"));
        EXPECT_TRUE(tryAllRegExp(regs, "("));
        EXPECT_TRUE(tryAllRegExp(regs, ";"));
        EXPECT_TRUE(tryAllRegExp(regs, "515151515"));
        EXPECT_TRUE(tryAllRegExp(regs, ">"));
        EXPECT_TRUE(tryAllRegExp(regs, "*"));

    }
}