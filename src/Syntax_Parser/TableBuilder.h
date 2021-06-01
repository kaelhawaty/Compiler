//
// Created by Abd Elkader on 6/1/2021.
//

#ifndef COMPILER_TABLEBUILDER_H
#define COMPILER_TABLEBUILDER_H


#include "Syntax_Utils.h"

class TableBuilder {
public:
    TableBuilder(const std::unordered_map<Symbol, std::vector<Production>> &rules,
                 const Syntax_Utils &syntaxUtils);

    std::unordered_map<Symbol,std::unordered_map<Symbol,Production>> getTable() const;

private:
    const std::unordered_map<Symbol, std::vector<Production>> &rules;
    const Syntax_Utils &syntaxUtils;
    std::unordered_map<Symbol,std::unordered_map<Symbol,Production>> table;

    void addRowToTable(const Symbol &nonTerminal, const std::vector<Production> &productions);
    void addProductionToRow(const Symbol &nonTerminal, const Symbol &terminal, const Production &production);
};


#endif //COMPILER_TABLEBUILDER_H
