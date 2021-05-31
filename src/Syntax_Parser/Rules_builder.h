//
// Created by hazem on 5/30/2021.
//

#include <unordered_map>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <tuple>
#include "../Parser/Utils/ParserUtils.h"
#include "Syntax_Analyzer.h"

#ifndef COMPILER_CFG_READER_H
#define COMPILER_CFG_READER_H

#endif //COMPILER_CFG_READER_H



class Rules_builder {
public:
    explicit Rules_builder(const std::string &inputFilePath);

    const std::unordered_map<Symbol, Rule> &getRules() const;

    const Symbol &getStartSymbol() const;

    bool fail() const;

private:
    void insert_new_definition(std::string &rule_def, std::unordered_map<std::string, bool> &defined_in_lhs);

    void eliminate_left_recursion();

    void eliminate_immediate_left_recursion(const std::string &LHS, const std::vector<Production> &RHS);

    bool is_left_dependent(const Production &prod, const std::string &prev_non_terminal);

    std::vector<Production> substitute(Production &curProd, std::vector<Production> prevProd);

    std::unordered_map<Symbol, Rule> rules;
    Symbol start_symbol;
    bool has_error;
};
