//
// Created by Karim on 6/2/2021.
//

#ifndef COMPILER_SYNTAX_TESTS_HELPER_H
#define COMPILER_SYNTAX_TESTS_HELPER_H

#include <algorithm>
#include <cctype>
#include <vector>
#include <string>
#include "../src/Syntax_Parser/Syntax_definitions.h"

static std::vector<Symbol> writeSymbols(const std::vector<std::string> &list) {
    std::vector<Symbol> ret;
    std::for_each(list.begin(), list.end(), [&](const std::string &symbol) {
        if (symbol[0] == '#') {
            ret.push_back({"#", Symbol::Type::EPSILON});
            return;
        }
        if (symbol[0] == '\'') {
            ret.push_back({symbol.substr(1, symbol.size() - 2), Symbol::Type::TERMINAL});
            return;
        }
        ret.push_back({symbol, Symbol::Type::NON_TERMINAL});
    });
    return ret;
}

// Usage: writeProductions({"'+' T R", "#"})
static std::vector<Production> writeProductions(const std::vector<std::string> &list) {
    std::vector<Production> ret;
    for (const auto &str: list) {
        std::istringstream iss(str);
        std::vector<std::string> split{std::istream_iterator<std::string>{iss},
                                       std::istream_iterator<std::string>{}};
        ret.push_back(writeSymbols(split));
    }
    return ret;
}

// Usage: writeRule("R", writeProductions({"+TR", "#"}))
static Rule writeRule(const std::string &lhs, const std::vector<Production> &prod) {
    return Rule(lhs, prod);
}

static bool isEqual(std::vector<Production> prod1, std::vector<Production> prod2) {
    sort(prod1.begin(), prod1.end());
    sort(prod2.begin(), prod2.end());
    return prod1 == prod2;
}

#endif //COMPILER_SYNTAX_TESTS_HELPER_H
