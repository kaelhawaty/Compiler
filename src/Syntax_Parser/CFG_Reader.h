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
};
