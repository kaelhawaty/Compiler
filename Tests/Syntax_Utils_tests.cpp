//
// Created by Mahmoudmohamed on 5/30/2021.
//

#include "gtest/gtest.h"
#include "../src/Synax_Parser/Syntax_Utils.h"


namespace Syntax_Utils_tests {

    void checkTerminalEquality(std::unordered_set<std::string> expected_terminals, Syntax_Utils::Terminal_set res_terminals) {
        EXPECT_EQ(res_terminals.size(), expected_terminals.size());
        for (const Symbol &s : res_terminals) {
            EXPECT_TRUE(expected_terminals.find(s.name) != expected_terminals.end());
        }
    }

    TEST(FirstFollowConstruction, sample1) {
        //Input :
        //E  -> TR
        //R  -> +T R| #
        //T  -> F Y
        //Y  -> *F Y | #
        //F  -> (E) | i
        std::unordered_map<std::string, std::vector<std::vector<Symbol>>> rules = {
                {"E", {{{"T", Type::NON_TERMINAL}, {"R", Type::NON_TERMINAL}}}},
                {"R", {{{"+", Type::TERMINAL}, {"T", Type::NON_TERMINAL}, {"R", Type::NON_TERMINAL}},
                              {{"Є", Type::EPSILON}}}},
                {"T", {{{"F", Type::NON_TERMINAL}, {"Y", Type::NON_TERMINAL}}}},
                {"Y", {{{"*", Type::TERMINAL},{"F", Type::NON_TERMINAL}, {"Y", Type::NON_TERMINAL}},
                              {{"Є", Type::EPSILON}}}},
                {"F", {{{"(", Type::TERMINAL}, {"E", Type::NON_TERMINAL}, {")", Type::TERMINAL}},
                              {{"i", Type::TERMINAL}}}}
        };
        Symbol first_symbol = {"E", Type::NON_TERMINAL};
        Syntax_Utils utils_syntax(rules, first_symbol);
        std::vector<std::string> non_terminals = {"E", "R", "T", "Y", "F"};
        std::vector<std::unordered_set<std::string>> firsts = {{"(", "i"},
                                                               {"+", "Є"},
                                                               {"(", "i"},
                                                               {"*", "Є"},
                                                               {"(", "i"}};

        std::vector<std::unordered_set<std::string>> follows = {{"$", ")"},
                                                                {"$", ")"},
                                                                {"+", "$", ")"},
                                                                {"+", "$", ")"},
                                                                {"*", "+", "$", ")"}};
        for (int i = 0; i < non_terminals.size(); i++) {
            Symbol symbol = {non_terminals[i], Type::NON_TERMINAL};
            Syntax_Utils::First_set curr_first = utils_syntax.first_of(symbol);
            Syntax_Utils::Follow_set curr_follow = utils_syntax.follow_of(symbol);

            checkTerminalEquality(firsts[i], curr_first);
            checkTerminalEquality(follows[i], curr_follow);
        }

    }

    TEST(FirstFollowConstruction, sample2) {
        //S -> aBDh
        //B -> cC
        //C -> bC | Є
        //D -> EF
        //E -> g | Є
        //F -> f | Є
        std::unordered_map<std::string, std::vector<std::vector<Symbol>>> rules = {
                {"S", {{{"a", Type::TERMINAL}, {"B", Type::NON_TERMINAL}, {"D", Type::NON_TERMINAL}, {"h", Type::TERMINAL}}}},
                {"B", {{{"c", Type::TERMINAL}, {"C", Type::NON_TERMINAL}}}},
                {"C", {{{"b", Type::TERMINAL}, {"C", Type::NON_TERMINAL}},
                              {{"Є", Type::EPSILON}}}},
                {"D", {{{"E", Type::NON_TERMINAL}, {"F", Type::NON_TERMINAL}}}},
                {"E", {{{"g", Type::TERMINAL}},
                              {{"Є", Type::EPSILON}}}},
                {"F", {{{"f", Type::TERMINAL}},
                              {{"Є", Type::EPSILON}}}}
        };
        Symbol first_symbol = {"S", Type::NON_TERMINAL};
        Syntax_Utils utils_syntax(rules, first_symbol);
        std::vector<std::string> non_terminals = {"S", "B", "C", "D", "E", "F"};
        std::vector<std::unordered_set<std::string>> firsts = {{"a"},
                                                               {"c"},
                                                               {"b", "Є"},
                                                               {"g", "f", "Є"},
                                                               {"g", "Є"},
                                                               {"f", "Є"}};

        std::vector<std::unordered_set<std::string>> follows = {{"$"},
                                                                {"g", "f", "h"},
                                                                {"g", "f", "h"},
                                                                {"h"},
                                                                {"f", "h"},
                                                                {"h"}};
        for (int i = 0; i < non_terminals.size(); i++) {
            Symbol symbol = {non_terminals[i], Type::NON_TERMINAL};
            Syntax_Utils::First_set curr_first = utils_syntax.first_of(symbol);
            Syntax_Utils::Follow_set curr_follow = utils_syntax.follow_of(symbol);

            checkTerminalEquality(firsts[i], curr_first);
            checkTerminalEquality(follows[i], curr_follow);
        }

    }

    TEST(FirstFollowConstruction, adding_undefined_non_terminal) {
        //S -> AQCB|Cbb|Ba
        //A -> da|BC
        //B-> g|Є
        //C-> h| Є
        std::unordered_map<std::string, std::vector<std::vector<Symbol>>> rules = {
                {"S", {{{"A", Type::NON_TERMINAL},{"Q", Type::NON_TERMINAL} ,{"C", Type::NON_TERMINAL}, {"B", Type::NON_TERMINAL}},
                              {{"C", Type::NON_TERMINAL}, {"b", Type::TERMINAL}, {"b", Type::TERMINAL}},
                              {{"B", Type::NON_TERMINAL}, {"a", Type::TERMINAL}}}},
                {"A", {{{"d", Type::TERMINAL}, {"a", Type::TERMINAL}},
                              {{"B", Type::NON_TERMINAL}, {"C", Type::NON_TERMINAL}}}},
                {"B", {{{"g", Type::TERMINAL}},
                              {{"Є", Type::EPSILON}}}},
                {"C", {{{"h", Type::TERMINAL}},
                              {{"Є", Type::EPSILON}}}}
        };
        Symbol first_symbol = {"S", Type::NON_TERMINAL};
        Syntax_Utils utils_syntax(rules, first_symbol);
        std::vector<std::string> non_terminals = {"S", "A", "B", "C"};
        std::vector<std::unordered_set<std::string>> firsts = {{"d", "g", "h", "Є", "b", "a"},
                                                               {"d", "g", "h", "Є"},
                                                               {"g", "Є"},
                                                               {"h", "Є"}};

        std::vector<std::unordered_set<std::string>> follows = {{"$"},
                                                                {"g", "$", "h"},
                                                                {"g", "a", "h", "$"},
                                                                {"h", "b", "g", "$"}};
        for (int i = 0; i < non_terminals.size(); i++) {
            Symbol symbol = {non_terminals[i], Type::NON_TERMINAL};
            Syntax_Utils::First_set curr_first = utils_syntax.first_of(symbol);
            Syntax_Utils::Follow_set curr_follow = utils_syntax.follow_of(symbol);

            checkTerminalEquality(firsts[i], curr_first);
            checkTerminalEquality(follows[i], curr_follow);
        }

    }
}