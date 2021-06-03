//
// Created by Abd Elkader on 6/1/2021.
//
#include "gtest/gtest.h"
#include "../src/Syntax_Parser/ParsingTable.h"
#include "Syntax_tests_helper.h"

namespace ParsingTable_tests{

    TEST(allProductions, sample1) {
        //Input :
        //E  -> TR
        //R  -> +T R| #
        //T  -> F Y
        //Y  -> *F Y | #
        //F  -> (E) | i
        std::unordered_map<Symbol, Rule> rules = {
                {{"E", Symbol::Type::NON_TERMINAL}, writeRule("E", writeProductions({"T R"}))},
                {{"R", Symbol::Type::NON_TERMINAL}, writeRule("R", writeProductions({"'+' T R", "#"}))},
                {{"T", Symbol::Type::NON_TERMINAL}, writeRule("T", writeProductions({"F Y"}))},
                {{"Y", Symbol::Type::NON_TERMINAL}, writeRule("Y", writeProductions({"'*' F Y", "#"}))},
                {{"F", Symbol::Type::NON_TERMINAL}, writeRule("F", writeProductions({"'(' E ')'", "'i'"}))}
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
            const Production &production = table.getProduction(nonTerminal,terminal);
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
            const Production &production = table.getProduction(nonTerminal,terminal);
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
            const Production &production = table.getProduction(nonTerminal,terminal);
            EXPECT_TRUE(production == SYNC_PRODUCTION);
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