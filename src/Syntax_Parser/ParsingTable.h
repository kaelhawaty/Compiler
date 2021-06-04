//
// Created by Abd Elkader on 6/1/2021.
//

#ifndef COMPILER_PARSINGTABLE_H
#define COMPILER_PARSINGTABLE_H


#include "Syntax_Utils.h"

class ParsingTable {
public:
    ParsingTable(const std::unordered_map<Symbol, Rule> &rules,
                 const Syntax_Utils &syntaxUtils);

    const Production& getProduction(const Symbol &nonTerminal, const Symbol &terminal) const;
    bool hasProduction(const Symbol &nonTerminal, const Symbol &terminal) const;
    bool fail() const;

private:
    std::unordered_map<Symbol,std::unordered_map<Symbol,Production>> table;
    bool has_error{};

    void addRowToTable(const Symbol &nonTerminal, const std::vector<Production> &productions,const Syntax_Utils &syntaxUtils);
    void addProductionToRow(const Symbol &nonTerminal, const Symbol &terminal, const Production &production);
};


#endif //COMPILER_PARSINGTABLE_H
