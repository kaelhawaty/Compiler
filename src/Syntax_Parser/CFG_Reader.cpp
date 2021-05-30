//
// Created by hazem on 5/30/2021.
//

#include <sstream>
#include "CFG_Reader.h"


const char RULE_SEPARATOR = '|';
const char RULE_START = '#';
const char RULE_ASSIGNMENT = '=';

/**
 * Coverts Grammar rules stored in a file to Deterministic State Automaton object.
 * @param inputFilePath path to file containing the CFG file of the given language.
 * @return CFG_Reader object containing the productions of the language and the start symbol.
 *
 */
CFG_Reader::CFG_Reader(std::string &inputFilePath) {
    std::fstream file;
    file.open(inputFilePath, std::ios::in);
    if (!file.is_open()) {
        std::cerr << "Invalid CFG file path\n";
        exit(-1);
    }
    std::string rule_def, line;
    getline(file, rule_def);
    while (getline(file, line)) {
        if (line[0] == RULE_START) {
            insert_new_definition(rule_def);
            rule_def = line;
        } else {
            rule_def += line;
        }
    }
    insert_new_definition(rule_def);
    file.close();
//    eliminate_left_recursion();
}

void CFG_Reader::insert_new_definition(std::string &rule_def) {
    std::istringstream iss(rule_def);
    std::string token, LHS;
    std::vector<std::string> RHS;
    iss >> token;
    assert(token.size() == 1 && token[0] == RULE_START);
    iss >> LHS >> token;
    assert(token.size() == 1 && token[0] == '=');
    while (iss >> token) {
        RHS.push_back(token);
    }
    Production current_prod;
    std::vector<Production> productions;
    for (std::string &token : RHS) {
        if (token.size() == 1 && token[0] == '|') {
            productions.push_back(current_prod);
            current_prod.clear();
        }else {
            Symbol symbol;
            if (token[0] == '\'' && token[token.size() - 1] == '\'') {
                symbol.name = token.substr(1, token.size()-2);
                symbol.type = Type::TERMINAL;
            }else if(token.size() == 2 && token[0] == '\\' && token[1] == 'L') {
                symbol.name = token;
                symbol.type = Type::EPSILON;
            }else {
                symbol.name = token;
                symbol.type = Type::NON_TERMINAL;
            }
            current_prod.push_back(symbol);
        }
    }
    productions.push_back(current_prod);
    rules[LHS].insert(rules[LHS].end(), productions.begin(), productions.end());
    if (start_symbol.empty()) {
        start_symbol = LHS;
    }
}
/*

void CFG_Reader::eliminate_left_recursion() {
    std::vector<std::vector<Symbol>> new_rules;
    for (auto &i : productions) {
        for (auto &j : productions) {
            if (j == i) {
                break;
            }
            std::vector<std::vector<Symbol>> expanded_prod;
            for (auto &prod : i.second) {
                if (is_left_dependent(prod, j.first)) {
                    auto substituted = substitute(prod, j.second);
                    expanded_prod.insert(expanded_prod.end(), substituted.begin(), substituted.end());
                }else {
                    expanded_prod.push_back(prod);
                }
            }
            i.second = expanded_prod;
        }
        eliminate_immediate_left_recursion(i.first, i.second, new_rules);
    }

}

void CFG_Reader::eliminate_immediate_left_recursion(const std::string &LHS, const std::vector<std::vector<Symbol>> &RHS, std::vector<std::vector<Symbol>> &new_rules) {
    std::vector<std::vector<Symbol>> start_with_LHS;
    std::vector<std::vector<Symbol>> doesnt_start_with_LHS;
    for (const auto &prod : RHS) {
        if (is_left_dependent(prod, LHS)) {
            start_with_LHS.push_back(prod);
        }else {
            doesnt_start_with_LHS.push_back(prod);
        }
    }

}

bool CFG_Reader::is_left_dependent(const std::vector<Symbol> &prod, const std::string &prev_non_terminal) {
    return prod.front().name == prev_non_terminal && prod.front().type == Type::NON_TERMINAL;
}

std::vector<std::vector<Symbol>> CFG_Reader::substitute(std::vector<Symbol> &curProd, std::vector<std::vector<Symbol>> prevProd) {
    curProd.erase(curProd.begin());
    for (auto &rule : prevProd) {
        rule.insert(rule.end(), curProd.begin(), curProd.end());
    }
    return prevProd;
}
 */