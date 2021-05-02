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
            components.emplace_back(std::make_pair(std::string(1, c), std::vector<component>{component(RED_DEF, std::string(1, c))}));
        }
        return components;
    }

    TEST(BuildingNFAFromChar, simpleTest) {
        ComponentParser componentParser;
        std::vector<std::pair<std::string, std::vector<component>>> components;
        components.emplace_back("a",std::vector<component>{component(RED_DEF, "a")});
        std::unordered_map<std::string, NFA> res = componentParser.regDefinitionsToNFAs(components);
        EXPECT_EQ(res.size(), 1);
        EXPECT_TRUE(MatchRegexp("a", res["a"]));
    }

    TEST(BuildingNFAsFromMultipleComponents, checkNormalConcat) {
        ComponentParser componentParser;
        std::vector<std::pair<std::string, std::vector<component>>> components = buildBasicComponents();

        components.emplace_back("id", std::vector<component>{component(RED_DEF, "M"),
                                                                  component(CONCAT),
                                                                  component(RED_DEF, "H")});
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

        components.emplace_back("id", std::vector<component>{component(RED_DEF, "M"),
                                                                  component(CONCAT),
                                                                  component(RED_DEF, "H"),
                                                                  component(OR),
                                                                  component(RED_DEF, "K"),
                                                                  component(CONCAT),
                                                                  component(RED_DEF, "O"),
                                                                  component(OR),
                                                                  component(RED_DEF, "Y")});
        std::unordered_map<std::string, NFA> res = componentParser.regDefinitionsToNFAs(components);
        EXPECT_EQ(res.size(), 128);
        EXPECT_FALSE(MatchRegexp("a", res["id"]));
        EXPECT_FALSE(MatchRegexp("M", res["id"]));
        EXPECT_FALSE(MatchRegexp("H", res["id"]));
        EXPECT_TRUE(MatchRegexp("MH", res["id"]));
        EXPECT_TRUE(MatchRegexp("KO", res["id"]));
        EXPECT_TRUE(MatchRegexp("Y", res["id"]));
        EXPECT_FALSE(MatchRegexp("YI", res["id"]));
    }

    TEST(BuildingNFAsFromMultipleComponents, checkConcatWithORWithBrackets) {
        ComponentParser componentParser;
        std::vector<std::pair<std::string, std::vector<component>>> components = buildBasicComponents();
        // M (H | K) O | Y
        components.emplace_back("id", std::vector<component>{component(RED_DEF, "M"),
                                                                  component(CONCAT),
                                                                  component(OPEN_BRACKETS),
                                                                  component(RED_DEF, "H"),
                                                                  component(OR),
                                                                  component(RED_DEF, "K"),
                                                                  component(CLOSE_BRACKETS),
                                                                  component(CONCAT),
                                                                  component(RED_DEF, "O"),
                                                                  component(OR),
                                                                  component(RED_DEF, "Y")});
        std::unordered_map<std::string, NFA> res = componentParser.regDefinitionsToNFAs(components);
        EXPECT_EQ(res.size(), 128);
        EXPECT_FALSE(MatchRegexp("a", res["id"]));
        EXPECT_FALSE(MatchRegexp("M", res["id"]));
        EXPECT_FALSE(MatchRegexp("H", res["id"]));
        EXPECT_FALSE(MatchRegexp("MH", res["id"]));
        EXPECT_FALSE(MatchRegexp("KO", res["id"]));
        EXPECT_TRUE(MatchRegexp("Y", res["id"]));
        EXPECT_FALSE(MatchRegexp("YI", res["id"]));
        EXPECT_TRUE(MatchRegexp("MHO", res["id"]));
        EXPECT_TRUE(MatchRegexp("MKO", res["id"]));
        EXPECT_FALSE(MatchRegexp("MHKO", res["id"]));
    }

    TEST(BuildingNFAsFromMultipleComponents, checkConcatWithORWithBracketsWithClosures) {
        ComponentParser componentParser;
        std::vector<std::pair<std::string, std::vector<component>>> components = buildBasicComponents();
        // M (H | K)+ O | Y*
        components.emplace_back("id", std::vector<component>{component(RED_DEF, "M"),
                                                                  component(CONCAT),
                                                                  component(OPEN_BRACKETS),
                                                                  component(RED_DEF, "H"),
                                                                  component(OR),
                                                                  component(RED_DEF, "K"),
                                                                  component(CLOSE_BRACKETS),
                                                                  component(POS_CLOSURE),
                                                                  component(CONCAT),
                                                                  component(RED_DEF, "O"),
                                                                  component(OR),
                                                                  component(RED_DEF, "Y"),
                                                                  component(KLEENE_CLOSURE)});
        std::unordered_map<std::string, NFA> res = componentParser.regDefinitionsToNFAs(components);
        EXPECT_EQ(res.size(), 128);
        EXPECT_FALSE(MatchRegexp("a", res["id"]));
        EXPECT_FALSE(MatchRegexp("M", res["id"]));
        EXPECT_FALSE(MatchRegexp("H", res["id"]));
        EXPECT_FALSE(MatchRegexp("MH", res["id"]));
        EXPECT_FALSE(MatchRegexp("KO", res["id"]));
        EXPECT_TRUE(MatchRegexp("Y", res["id"]));
        EXPECT_FALSE(MatchRegexp("YI", res["id"]));
        EXPECT_TRUE(MatchRegexp("MHO", res["id"]));
        EXPECT_TRUE(MatchRegexp("MKO", res["id"]));
        EXPECT_TRUE(MatchRegexp("MHKO", res["id"]));
        EXPECT_TRUE(MatchRegexp("YYYYYYYYYYYYYYYYYY", res["id"]));
        EXPECT_TRUE(MatchRegexp("", res["id"]));
        EXPECT_TRUE(MatchRegexp("MKKKKKKO", res["id"]));
        EXPECT_TRUE(MatchRegexp("MHKHKKHKHKHO", res["id"]));
        EXPECT_TRUE(MatchRegexp("MHHO", res["id"]));
    }

    TEST(BuildingNFAsFromMultipleComponents, checkTO) {
        ComponentParser componentParser;
        std::vector<std::pair<std::string, std::vector<component>>> components = buildBasicComponents();
        // M (H | K)+ O | Y*
        components.emplace_back("id", std::vector<component>{component(RED_DEF, "A"),
                                                                   component(TO),
                                                                   component(RED_DEF, "Z")});
        std::unordered_map<std::string, NFA> res = componentParser.regDefinitionsToNFAs(components);
        EXPECT_TRUE(MatchRegexp("A", res["id"]));
    }


    TEST(BuildingNFAsFromMultipleComponents, Exception) {
        ComponentParser componentParser;
        std::vector<std::pair<std::string, std::vector<component>>> components = buildBasicComponents();
        // M (H | K)+ O | Y*
        components.emplace_back("id", std::vector<component>{component(OPEN_BRACKETS),
                                                             component(OPEN_BRACKETS),
                                                             component(RED_DEF, "Z")});
        std::unordered_map<std::string, NFA> res = componentParser.regDefinitionsToNFAs(components);
        EXPECT_EQ(res.size(), 127);
    }

}