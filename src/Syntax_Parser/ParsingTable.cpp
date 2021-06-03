//
// Created by Abd Elkader on 6/1/2021.
//
#include <iostream>
#include "ParsingTable.h"

ParsingTable::ParsingTable(const std::unordered_map<Symbol, Rule> &rules,
                           const Syntax_Utils &syntaxUtils){

    for(const auto &[nonTerminal, productions] : rules)
        addRowToTable(nonTerminal, productions,syntaxUtils);

}

bool ParsingTable::hasProduction(const Symbol &nonTerminal, const Symbol &terminal) const {
    if(table.find(nonTerminal) != table.end()){
        const std::unordered_map<Symbol,Production> &row = table.at(nonTerminal);
        return row.find(terminal) != row.end();
    }
    return false;
}

const Production& ParsingTable::getProduction(const Symbol &nonTerminal, const Symbol &terminal) const {
    return table.at(nonTerminal).at(terminal);
}


void ParsingTable::addRowToTable(const Symbol &nonTerminal, const std::vector<Production> &productions, const Syntax_Utils &syntaxUtils) {

    // store which production to use for the follow set of this non terminal
    Production followProduction = {};
    for(const auto &production : productions){
        std::unordered_set<Symbol> productionFirst = syntaxUtils.first_of(production);
        if(productionFirst.count(eps_symbol)){
            if(followProduction.empty()){
                followProduction = production;
            }else{
                std::cerr << "More than one production for non_terminal = " << nonTerminal.name << " evaluates to epsilon.\n";
                has_error = true;
            }
            productionFirst.erase(eps_symbol);
        }
        for(auto &terminal : productionFirst){
            addProductionToRow(nonTerminal, terminal, production);
        }
    }

    if(followProduction.empty())
        followProduction = SYNC_PRODUCTION;

    std::unordered_set<Symbol> nonTerminalFollow = syntaxUtils.follow_of(nonTerminal);
    for(auto &terminal : nonTerminalFollow){
        addProductionToRow(nonTerminal, terminal, followProduction);
    }

}

void ParsingTable::addProductionToRow(const Symbol &nonTerminal, const Symbol &terminal, const Production &production) {
    std::unordered_map<Symbol, Production> &row = table[nonTerminal];
    if(row.find(terminal) == row.end()){
        row[terminal] = production;
    }else if(production != SYNC_PRODUCTION){
        std::cerr << "More than one production at entry of non_terminal = " << nonTerminal.name << " and terminal = " << terminal.name << " .\n";
        has_error = true;
    }
}

bool ParsingTable::fail() const {
    return has_error;
}

