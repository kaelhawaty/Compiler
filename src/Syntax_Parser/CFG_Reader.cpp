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
}

void CFG_Reader::insert_new_definition(std::string &rule_def) {
    std::istringstream iss(rule_def);
    std::string token, LHS;
    std::vector<std::string> RHS;
    iss >> token;
    assert(token.size() == 1 && token[0] == RULE_START);
    iss >> LHS >> token;
    assert(token.size() == 1 && token[0] == RULE_ASSIGNMENT);
    while (iss >> token) {
        RHS.push_back(token);
    }
    Production current_prod;
    std::vector<Production> productions;
    for (std::string &token : RHS) {
        if (token.size() == 1 && token[0] == RULE_SEPARATOR) {
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
