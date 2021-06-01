//
// Created by Abd Elkader on 6/1/2021.
//
#include <iostream>
#include "TableBuilder.h"

TableBuilder::TableBuilder(const std::unordered_map<Symbol, std::vector<Production>> &rules,
                           const Syntax_Utils &syntaxUtils):
                           rules(rules), syntaxUtils(syntaxUtils){

    for(const auto &[nonTerminal, productions] : this->rules)
        addRowToTable(nonTerminal, productions);

}

std::unordered_map<Symbol, std::unordered_map<Symbol, Production>> TableBuilder::getTable() const {
    return table;
}

void TableBuilder::addRowToTable(const Symbol &nonTerminal, const std::vector<Production> &productions) {

    // store which production to use for the follow set of this non terminal
    Production followProduction = {};
    for(const auto &production : productions){
        std::unordered_set<Symbol> productionFirst = syntaxUtils.first_of(production);
        if(productionFirst.count(eps_symbol)){
            if(followProduction.empty()){
                followProduction = production;
            }else{
                std::cerr << "More than one production for non_terminal = " << nonTerminal.name << " evaluates to epsilon.\n";
            }
        }
        productionFirst.erase(eps_symbol);
        for(auto &terminal : productionFirst){
            addProductionToRow(nonTerminal, terminal, production);
        }
    }

    if(followProduction.empty())
        followProduction = SYNC;

    std::unordered_set<Symbol> nonTerminalFollow = syntaxUtils.follow_of(nonTerminal);
    for(auto &terminal : nonTerminalFollow){
        addProductionToRow(nonTerminal, terminal, followProduction);
    }

}

void TableBuilder::addProductionToRow(const Symbol &nonTerminal, const Symbol &terminal, const Production &production) {
    if(table[nonTerminal].find(terminal) == table[nonTerminal].end()){
        table[nonTerminal][terminal] = production;
    }else if(production != SYNC){
        std::cerr << "More than one production at entry of non_terminal = " << nonTerminal.name << " and terminal = " << terminal.name << " .\n";
    }
}

