//
// Created by Mahmoudmohamed on 5/30/2021.
//

#include "gtest/gtest.h"
#include "../src/Syntax_Parser/Syntax_Utils.h"


namespace Syntax_Utils_tests {

    void checkTerminalEquality(std::unordered_set<std::string> expected_terminals, const Syntax_Utils::Terminal_set& res_terminals) {
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
        std::unordered_map<Symbol, Rule> rules = {
                {{"E", Symbol::Type::NON_TERMINAL}, {{{"T", Symbol::Type::NON_TERMINAL}, {"R", Symbol::Type::NON_TERMINAL}}}},
                {{"R", Symbol::Type::NON_TERMINAL}, {{{"+", Symbol::Type::TERMINAL}, {"T", Symbol::Type::NON_TERMINAL}, {"R", Symbol::Type::NON_TERMINAL}},
                              {{"Є", Symbol::Type::EPSILON}}}},
                {{"T", Symbol::Type::NON_TERMINAL}, {{{"F", Symbol::Type::NON_TERMINAL}, {"Y", Symbol::Type::NON_TERMINAL}}}},
                {{"Y", Symbol::Type::NON_TERMINAL}, {{{"*", Symbol::Type::TERMINAL},{"F", Symbol::Type::NON_TERMINAL}, {"Y", Symbol::Type::NON_TERMINAL}},
                              {{"Є", Symbol::Type::EPSILON}}}},
                {{"F", Symbol::Type::NON_TERMINAL}, {{{"(", Symbol::Type::TERMINAL}, {"E", Symbol::Type::NON_TERMINAL}, {")", Symbol::Type::TERMINAL}},
                              {{"i", Symbol::Type::TERMINAL}}}}
        };
        Symbol first_symbol = {"E", Symbol::Type::NON_TERMINAL};
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
            Symbol symbol = {non_terminals[i], Symbol::Type::NON_TERMINAL};
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
        std::unordered_map<Symbol, Rule> rules = {
                {{"S", Symbol::Type::NON_TERMINAL}, {{{"a", Symbol::Type::TERMINAL}, {"B", Symbol::Type::NON_TERMINAL}, {"D", Symbol::Type::NON_TERMINAL}, {"h", Symbol::Type::TERMINAL}}}},
                {{"B", Symbol::Type::NON_TERMINAL}, {{{"c", Symbol::Type::TERMINAL}, {"C", Symbol::Type::NON_TERMINAL}}}},
                {{"C", Symbol::Type::NON_TERMINAL}, {{{"b", Symbol::Type::TERMINAL}, {"C", Symbol::Type::NON_TERMINAL}},
                              {{"Є", Symbol::Type::EPSILON}}}},
                {{"D", Symbol::Type::NON_TERMINAL}, {{{"E", Symbol::Type::NON_TERMINAL}, {"F", Symbol::Type::NON_TERMINAL}}}},
                {{"E", Symbol::Type::NON_TERMINAL}, {{{"g", Symbol::Type::TERMINAL}},
                              {{"Є", Symbol::Type::EPSILON}}}},
                {{"F", Symbol::Type::NON_TERMINAL}, {{{"f", Symbol::Type::TERMINAL}},
                              {{"Є", Symbol::Type::EPSILON}}}}
        };
        Symbol first_symbol = {"S", Symbol::Type::NON_TERMINAL};
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
            Symbol symbol = {non_terminals[i], Symbol::Type::NON_TERMINAL};
            Syntax_Utils::First_set curr_first = utils_syntax.first_of(symbol);
            Syntax_Utils::Follow_set curr_follow = utils_syntax.follow_of(symbol);

            checkTerminalEquality(firsts[i], curr_first);
            checkTerminalEquality(follows[i], curr_follow);
        }

    }

    TEST(FirstFollowConstruction, adding_undefined_non_terminal) {
        //S -> ACB|Cbb|Ba
        //A -> da|BC
        //B-> g|Є
        //C-> h| Є
        std::unordered_map<Symbol, Rule> rules = {
                {{"S", Symbol::Type::NON_TERMINAL}, {{{"A", Symbol::Type::NON_TERMINAL}, {"C", Symbol::Type::NON_TERMINAL}, {"B", Symbol::Type::NON_TERMINAL}},
                              {{"C", Symbol::Type::NON_TERMINAL}, {"b", Symbol::Type::TERMINAL}, {"b", Symbol::Type::TERMINAL}},
                              {{"B", Symbol::Type::NON_TERMINAL}, {"a", Symbol::Type::TERMINAL}}}},
                {{"A", Symbol::Type::NON_TERMINAL}, {{{"d", Symbol::Type::TERMINAL}, {"a", Symbol::Type::TERMINAL}},
                              {{"B", Symbol::Type::NON_TERMINAL}, {"C", Symbol::Type::NON_TERMINAL}}}},
                {{"B", Symbol::Type::NON_TERMINAL}, {{{"g", Symbol::Type::TERMINAL}},
                              {{"Є", Symbol::Type::EPSILON}}}},
                {{"C", Symbol::Type::NON_TERMINAL}, {{{"h", Symbol::Type::TERMINAL}},
                              {{"Є", Symbol::Type::EPSILON}}}}
        };
        Symbol first_symbol = {"S", Symbol::Type::NON_TERMINAL};
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
            Symbol symbol = {non_terminals[i], Symbol::Type::NON_TERMINAL};
            Syntax_Utils::First_set curr_first = utils_syntax.first_of(symbol);
            Syntax_Utils::Follow_set curr_follow = utils_syntax.follow_of(symbol);

            checkTerminalEquality(firsts[i], curr_first);
            checkTerminalEquality(follows[i], curr_follow);
        }

    }
}