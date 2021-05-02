//
// Created by Mahmoudmohamed on 5/1/2021.
//
#include "gtest/gtest.h"
#include <vector>
#include <climits>
#include "../src/NFA/NFA.h"
#include "../src/NFA/NFA_Builder.h"
#include "../src/Parser/Component.h"
#include "../src/Parser/ComponentParser.h"

namespace ComponentParser_tests {

    bool MatchRegexp(const std::string &s, const NFA &nfa) {
        NFA::Set st{nfa.get_start()};
        st = E_closure(st);
        for (char c: s) {
            st = E_closure(Move(st, c));
        }
        return st.count(nfa.get_end()) != 0;
    }

    std::vector<std::pair<std::string, std::vector<component>>> buildBasicComponents() {
        std::vector<std::pair<std::string, std::vector<component>>> components;
        for (char c = 0; c < CHAR_MAX; c++) {
            components.emplace_back(std::string{c}, std::vector<component>{{RED_DEF, std::string{c}}});
        }
        return components;
    }

    TEST(BuildingNFAFromChar, simpleTest) {
        ComponentParser componentParser;
        std::vector<std::pair<std::string, std::vector<component>>> components;
        components.emplace_back("a",std::vector<component>{{RED_DEF, "a"}});
        std::unordered_map<std::string, NFA> res = componentParser.regDefinitionsToNFAs(components);
        EXPECT_EQ(res.size(), 1);
        EXPECT_TRUE(MatchRegexp("a", res["a"]));
    }

    TEST(BuildingNFAsFromMultipleComponents, checkNormalConcat) {
        ComponentParser componentParser;
        std::vector<std::pair<std::string, std::vector<component>>> components = buildBasicComponents();

        components.emplace_back("id", std::vector<component>{{RED_DEF, "M"},
                                                             {CONCAT, ""},
                                                             {RED_DEF, "H"}});
        std::unordered_map<std::string, NFA> res = componentParser.regDefinitionsToNFAs(components);
        EXPECT_EQ(res.size(), 128);
        EXPECT_FALSE(MatchRegexp("a", res["id"]));
        EXPECT_FALSE(MatchRegexp("M", res["id"]));
        EXPECT_FALSE(MatchRegexp("H", res["id"]));
        EXPECT_TRUE(MatchRegexp("MH", res["id"]));
    }

    TEST(BuildingNFAsFromMultipleComponents, checkConcatWithOR) {
        ComponentParser componentParser;
        std::vector<std::pair<std::string, std::vector<component>>> components = buildBasicComponents();

        components.emplace_back("id", std::vector<component>{{RED_DEF, "M"},
                                                             {CONCAT, ""},
                                                             {RED_DEF, "H"},
                                                             {OR, ""},
                                                             {RED_DEF, "K"},
                                                             {CONCAT, ""},
                                                             {RED_DEF, "O"},
                                                             {OR, ""},
                                                             {RED_DEF, "Y"}});
        std::unordered_map<std::string, NFA> res = componentParser.regDefinitionsToNFAs(components);
        EXPECT_EQ(res.size(), 128);
        std::vector<std::string> regsExps{"a", "M", "H", "MH", "KO", "Y", "YI"};
        std::vector<bool> want{false, false, false, true, true, true, false};
        for (int i = 0; i < regsExps.size(); i++) {
            EXPECT_EQ(MatchRegexp(regsExps[i], res["id"]), want[i]) << "Test case #" << i << ": " << regsExps[i];
        }
    }

    TEST(BuildingNFAsFromMultipleComponents, checkConcatWithORWithBrackets) {
        ComponentParser componentParser;
        std::vector<std::pair<std::string, std::vector<component>>> components = buildBasicComponents();
        // M (H | K) O | Y
        components.emplace_back("id", std::vector<component>{{RED_DEF, "M"},
                                                             {CONCAT, ""},
                                                             {OPEN_BRACKETS, ""},
                                                             {RED_DEF, "H"},
                                                             {OR, ""},
                                                             {RED_DEF, "K"},
                                                             {CLOSE_BRACKETS, ""},
                                                             {CONCAT, ""},
                                                             {RED_DEF, "O"},
                                                             {OR, ""},
                                                             {RED_DEF, "Y"}});
        std::unordered_map<std::string, NFA> res = componentParser.regDefinitionsToNFAs(components);
        EXPECT_EQ(res.size(), 128);
        std::vector<std::string> regsExps{"a", "M", "H", "MH", "KO", "Y", "YI", "MHO", "MKO", "MHKO"};
        std::vector<bool> want{false, false, false, false, false, true, false, true, true, false};
        for (int i = 0; i < regsExps.size(); i++) {
            EXPECT_EQ(MatchRegexp(regsExps[i], res["id"]), want[i]) << "Test case #" << i << ": " << regsExps[i];
        }
    }

    TEST(BuildingNFAsFromMultipleComponents, checkConcatWithORWithBracketsWithClosures) {
        ComponentParser componentParser;
        std::vector<std::pair<std::string, std::vector<component>>> components = buildBasicComponents();
        // M (H | K)+ O | Y*
        components.emplace_back("id", std::vector<component>{{RED_DEF, "M"},
                                                             {CONCAT, ""},
                                                             {OPEN_BRACKETS, ""},
                                                             {RED_DEF, "H"},
                                                             {OR, ""},
                                                             {RED_DEF, "K"},
                                                             {CLOSE_BRACKETS, ""},
                                                             {POS_CLOSURE, ""},
                                                             {CONCAT, ""},
                                                             {RED_DEF, "O"},
                                                             {OR, ""},
                                                             {RED_DEF, "Y"},
                                                             {KLEENE_CLOSURE, ""}});
        std::unordered_map<std::string, NFA> res = componentParser.regDefinitionsToNFAs(components);
        EXPECT_EQ(res.size(), 128);
        std::vector<std::string> regsExps{"a", "M", "H", "MH", "KO", "Y", "YI", "MHO", "MKO",
                                          "MHKO", "YYYYYYYYYYYYYYYYYY", "", "MKKKKKKO", "MHKHKKHKHKHO",
                                          "MHHO"};
        std::vector<bool> want{false, false, false, false, false, true, false, true, true, true, true,
                               true, true, true, true};
        for (int i = 0; i < regsExps.size(); i++) {
            EXPECT_EQ(MatchRegexp(regsExps[i], res["id"]), want[i]) << "Test case #" << i << ": " << regsExps[i];
        }
    }

    TEST(BuildingNFAsFromMultipleComponents, checkTO) {
        ComponentParser componentParser;
        std::vector<std::pair<std::string, std::vector<component>>> components = buildBasicComponents();
        // M (H | K)+ O | Y*
        components.emplace_back("id", std::vector<component>{{RED_DEF, "A"},
                                                             {TO, ""},
                                                             {RED_DEF, "Z"}});
        std::unordered_map<std::string, NFA> res = componentParser.regDefinitionsToNFAs(components);
        EXPECT_TRUE(MatchRegexp("A", res["id"]));
    }


    TEST(BuildingNFAsFromMultipleComponents, Exception) {
        ComponentParser componentParser;
        std::vector<std::pair<std::string, std::vector<component>>> components = buildBasicComponents();
        // M (H | K)+ O | Y*
        components.emplace_back("id", std::vector<component>{{OPEN_BRACKETS, ""},
                                                             {OPEN_BRACKETS, ""},
                                                             {RED_DEF, "Z"}});
        std::unordered_map<std::string, NFA> res = componentParser.regDefinitionsToNFAs(components);
        EXPECT_EQ(res.size(), 127);
    }

}