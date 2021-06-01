//
// Created by Abd Elkader on 6/1/2021.
//
#include "gtest/gtest.h"
#include "../src/Syntax_Parser/ParsingTable.h"

namespace ParsingTable_tests{

    TEST(allProductions, sample1) {
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

        ParsingTable table = ParsingTable(rules,utils_syntax);
        //E <$,SYNC> <),SYNC> <(,TR> <i,TR>
        //R <),Є> <+,+TR> <$,Є>
        //T <+,SYNC> <),SYNC> <$,SYNC> <(,FY> <i,FY>
        //F <+,SYNC> <*,SYNC> <$,SYNC> <),SYNC> <(,(E)> <i,i>
        //Y <+,Є> <$,Є> <*,*FY> <),Є>
        std::vector<std::vector<std::string>> presentEntries ={
                {"E","(","TR"}, {"E","i","TR"},
                {"R","+","+TR"},
                {"T","(","FY"}, {"T","i","FY"},
                {"F","(","(E)"}, {"F","i","i"},
                {"Y","*","*FY"}
        };
        for(auto &entry : presentEntries){
            Symbol nonTerminal = {entry[0],Symbol::Type ::NON_TERMINAL};
            Symbol terminal = {entry[1],Symbol::Type ::TERMINAL};
            EXPECT_TRUE(table.hasProduction(nonTerminal,terminal));
            Production  production = table.getProduction(nonTerminal,terminal);
            EXPECT_TRUE(production.size() == entry[2].size());
            for(int i=0 ; i< production.size() ; i++){
                EXPECT_TRUE(production[i].name.size() == 1);
                EXPECT_TRUE(production[i].name[0] == entry[2][i]);
            }
        }

        std::vector<std::vector<std::string>> epsilonEntries ={
                {"R",")"}, {"R","$"},
                {"Y","+"}, {"Y","$"}, {"Y",")"}
        };
        for(auto &entry : epsilonEntries){
            Symbol nonTerminal = {entry[0],Symbol::Type ::NON_TERMINAL};
            Symbol terminal = {entry[1],Symbol::Type ::TERMINAL};
            EXPECT_TRUE(table.hasProduction(nonTerminal,terminal));
            Production  production = table.getProduction(nonTerminal,terminal);
            EXPECT_TRUE(production.size() == 1);
            EXPECT_TRUE(production[0] == eps_symbol);
        }
        std::vector<std::vector<std::string>> syncEntries ={
                {"E",")"}, {"E","$"},
                {"T","+"}, {"T","$"}, {"T",")"},
                {"F","+"}, {"F","$"}, {"F",")"}, {"F","*"}
        };
        for(auto &entry :syncEntries){
            Symbol nonTerminal = {entry[0],Symbol::Type ::NON_TERMINAL};
            Symbol terminal = {entry[1],Symbol::Type ::TERMINAL};
            EXPECT_TRUE(table.hasProduction(nonTerminal,terminal));
            Production  production = table.getProduction(nonTerminal,terminal);
            EXPECT_TRUE(production == SYNC);
        }
        std::vector<std::vector<std::string>> errorEntries ={
                {"E","+"}, {"E","*"},
                {"R","*"}, {"R","("}, {"R","i"},
                {"T","*"},
                {"Y","("}, {"Y","i"}
        };
        for(auto &entry :errorEntries){
            Symbol nonTerminal = {entry[0],Symbol::Type ::NON_TERMINAL};
            Symbol terminal = {entry[1],Symbol::Type ::TERMINAL};
            EXPECT_FALSE(table.hasProduction(nonTerminal,terminal));
        }
    }
}