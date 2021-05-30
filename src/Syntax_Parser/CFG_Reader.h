//
// Created by hazem on 5/30/2021.
//

#include <unordered_map>
#include <vector>
#include <fstream>
#include <iostream>
#include <cassert>
#include "../Parser/Utils/ParserUtils.h"
#ifndef COMPILER_CFG_READER_H
#define COMPILER_CFG_READER_H

#endif //COMPILER_CFG_READER_H



class CFG_Reader {
public:
    enum class Type {
        TERMINAL,
        NON_TERMINAL,
        EPSILON
    };
    struct Symbol {
        std::string name;
        Type type;
    };
    using Production = std::vector<Symbol>;
    std::unordered_map<std::string, std::vector<Production>> rules;
    std::string start_symbol;
    explicit CFG_Reader(std::string &inputFilePath);
private:
    void insert_new_definition(std::string &rule_def);
    void eliminate_left_recursion();
    void eliminate_immediate_left_recursion(const std::string &LHS, const std::vector<std::vector<Symbol>> &RHS, std::vector<std::vector<Symbol>> &new_rules);
    bool is_left_dependent(const std::vector<Symbol> &prod, const std::string &prev_non_terminal);
    std::vector<std::vector<Symbol>> substitute(std::vector<Symbol> &curProd, std::vector<std::vector<Symbol>> prevProd);
};
