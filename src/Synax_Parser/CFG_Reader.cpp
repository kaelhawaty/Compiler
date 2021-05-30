//
// Created by hazem on 5/30/2021.
//

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
    int line_number = 1;
    while (getline(file, line)) {
        if (line[0] == RULE_START) {
            insert_new_definition(rule_def, line_number);
            rule_def = line;
        } else {
            rule_def += line;
        }
        line_number++;
    }
    insert_new_definition(rule_def, line_number);
    file.close();
}

void CFG_Reader::insert_new_definition(std::string &rule_def, const int line_number) {
    assert(rule_def[0] == RULE_START);
    auto pos = rule_def.find(RULE_ASSIGNMENT);
    if (pos == std::string::npos) {
        std::cerr << "Error at line " << line_number << ": Assignment operator is missing\n";
        exit(-1);
    }
    std::string LHS = trim(rule_def.substr(1, pos - 1));
    std::string RHS = trim(rule_def.substr(pos + 1));
    auto rules = parse_production_rules(RHS, line_number);
    productions[LHS].insert(productions[LHS].end(), rules.begin(), rules.end());
    if (start_symbol.empty()) {
        start_symbol = LHS;
    }
}

std::vector<std::vector<Symbol>> CFG_Reader::parse_production_rules(std::string &RHS, const int line_number) {
    std::vector<std::vector<Symbol>> rules;
    std::string rule;
    for (int index = 0; index < RHS.size(); ++index) {
        if (RHS[index] == RULE_SEPARATOR) {
            rule = trim(rule);
            if (rule.empty()) {
                std::cerr << "Error at line " << line_number << ": Production can't be empty\n";
                exit(-1);
            }
            rules.push_back(parse_rule(rule, line_number));
            rule = "";
        } else {
            if (RHS[index] == '\\') {
                index++;
            }
            rule += RHS[index];
        }
    }
    if (rule.empty()) {
        std::cerr << "Error at line " << line_number << ": Production can't be empty\n";
        exit(-1);
    }
    rules.push_back(parse_rule(rule, line_number));
    return rules;
}

std::vector<Symbol> CFG_Reader::parse_rule(std::string &rule, const int line_number) {
    std::vector<Symbol> symbols;
    for (int index = 0; index < rule.size(); ++index) {
        if (rule[index] == ' ') {
            continue;
        }
        std::string symbol;
        if (rule[index] == '\'') {
            index++;
            while (index < rule.size() && rule[index] != '\'') {
                symbol += rule[index++];
            }
            if (symbol.empty()) {
                std::cerr << "Error at line " << line_number << ": Terminal symbol can't be empty\n";
                exit(-1);
            }
            symbols.push_back({symbol, Type::TERMINAL});
        } else if (index < rule.size() - 1 && rule[index] == '\\' && rule[index + 1] == 'L') {
            symbols.push_back({"\\L", Type::EPSILON});
            index++;
        } else {
            while (index < rule.size() && rule[index] != ' ') {
                symbol += rule[index++];
            }
            symbols.push_back({symbol, Type::NON_TERMINAL});
        }
    }
    return symbols;
}