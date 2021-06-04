//
// Created by Mahmoudmohamed on 5/30/2021.
//

#include "gtest/gtest.h"
#include "../src/Syntax_Parser/Syntax_Utils.h"
#include "Syntax_tests_helper.h"


namespace Syntax_Utils_tests {

    void checkTerminalEquality(std::unordered_set<std::string> expected_terminals, const Syntax_Utils::Terminal_set& res_terminals) {
        EXPECT_EQ(res_terminals.size(), expected_terminals.size());
        for (const Symbol &s : res_terminals) {
            EXPECT_TRUE(expected_terminals.find(s.name) != expected_terminals.end());
        }
    }

    TEST(FirstFollowConstruction, sample1) {
        //Input :
        //E  -> T R
        //R  -> '+' T R| #
        //T  -> F Y
        //Y  -> '*' F Y | #
        //F  -> '(' E ')' | 'i'
        std::unordered_map<Symbol, Rule> rules = {
                {{"E", Symbol::Type::NON_TERMINAL}, writeRule("E", writeProductions({"T R"}))},
                {{"R", Symbol::Type::NON_TERMINAL}, writeRule("R", writeProductions({"'+' T R", "#"}))},
                {{"T", Symbol::Type::NON_TERMINAL}, writeRule("T", writeProductions({"F Y"}))},
                {{"Y", Symbol::Type::NON_TERMINAL}, writeRule("Y", writeProductions({"'*' F Y", "#"}))},
                {{"F", Symbol::Type::NON_TERMINAL}, writeRule("F", writeProductions({"'(' E ')'", "'i'"}))}
        };
        Symbol first_symbol = {"E", Symbol::Type::NON_TERMINAL};
        Syntax_Utils utils_syntax(rules, first_symbol);
        std::vector<std::string> non_terminals = {"E", "R", "T", "Y", "F"};
        std::vector<std::unordered_set<std::string>> firsts = {{"(", "i"},
                                                               {"+", "#"},
                                                               {"(", "i"},
                                                               {"*", "#"},
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
        //S -> 'a' B D h
        //B -> 'c' C
        //C -> 'b' C | #
        //D -> E F
        //E -> 'g' | #
        //F -> 'f' | #
        std::unordered_map<Symbol, Rule> rules = {
                {{"S", Symbol::Type::NON_TERMINAL}, writeRule("S", writeProductions({"'a' B D 'h'"}))},
                {{"B", Symbol::Type::NON_TERMINAL}, writeRule("B", writeProductions({"'c' C"}))},
                {{"C", Symbol::Type::NON_TERMINAL}, writeRule("C", writeProductions({"'b' C", "#"}))},
                {{"D", Symbol::Type::NON_TERMINAL}, writeRule("D", writeProductions({"E F"}))},
                {{"E", Symbol::Type::NON_TERMINAL}, writeRule("E", writeProductions({"'g'", "#"}))},
                {{"F", Symbol::Type::NON_TERMINAL}, writeRule("F", writeProductions({"'f'", "#"}))}
        };
        Symbol first_symbol = {"S", Symbol::Type::NON_TERMINAL};
        Syntax_Utils utils_syntax(rules, first_symbol);
        std::vector<std::string> non_terminals = {"S", "B", "C", "D", "E", "F"};
        std::vector<std::unordered_set<std::string>> firsts = {{"a"},
                                                               {"c"},
                                                               {"b", "#"},
                                                               {"g", "f", "#"},
                                                               {"g", "#"},
                                                               {"f", "#"}};

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
        //S -> A C B | C 'b' 'b' | B 'a'
        //A -> 'd' 'a'| B C
        //B-> 'g' | #
        //C-> 'h' | #
        std::unordered_map<Symbol, Rule> rules = {
                {{"S", Symbol::Type::NON_TERMINAL}, writeRule("S", writeProductions({"A B C", "C 'b' 'b'", "B 'a'"}))},
                {{"A", Symbol::Type::NON_TERMINAL}, writeRule("A", writeProductions({"'d' 'a'", "B C"}))},
                {{"B", Symbol::Type::NON_TERMINAL}, writeRule("B", writeProductions({"'g'", "#"}))},
                {{"C", Symbol::Type::NON_TERMINAL}, writeRule("B", writeProductions({"'h'", "#"}))}
        };
        Symbol first_symbol = {"S", Symbol::Type::NON_TERMINAL};
        Syntax_Utils utils_syntax(rules, first_symbol);
        std::vector<std::string> non_terminals = {"S", "A", "B", "C"};
        std::vector<std::unordered_set<std::string>> firsts = {{"d", "g", "h", "#", "b", "a"},
                                                               {"d", "g", "h", "#"},
                                                               {"g", "#"},
                                                               {"h", "#"}};

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