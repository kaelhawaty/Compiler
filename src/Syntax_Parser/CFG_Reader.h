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


namespace CFG_Reader {

    std::tuple<std::unordered_map<Symbol, Rule>, Symbol, bool> parse_input_file(std::string &inputFilePath);

    void insert_new_definition(std::string &rule_def, std::unordered_map<Symbol, Rule> &rules, Symbol &start_symbol,
                               bool &has_error, std::unordered_map<std::string, bool> &defined_in_lhs);
}