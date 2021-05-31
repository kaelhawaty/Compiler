//
// Created by hazem on 5/30/2021.
//

#include <unordered_map>
#include <vector>
#ifndef COMPILER_CFG_READER_H
#define COMPILER_CFG_READER_H

#endif //COMPILER_CFG_READER_H
enum class Type {
    TERMINAL,
    NON_TERMINAL,
    EPSILON
};
struct Symbol {
    std::string name;
    Type type;
    bool operator==(const Symbol &other_symbol) const {
        return (name == other_symbol.name && type == other_symbol.type);
    }
};
class CFG_Reader {
    std::unordered_map<std::string, std::vector<std::vector<Symbol>>> productions;
};
