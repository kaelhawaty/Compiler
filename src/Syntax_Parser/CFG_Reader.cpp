//
// Created by hazem on 5/30/2021.
//

#include "CFG_Reader.h"

const char RULE_SEPARATOR = '|';
const char RULE_START = '#';
const char RULE_ASSIGNMENT = '=';
const char DASH = '\'';

/**
 *  Reads the CFG rules file line by line and parse each rule definition to its groups
 *  of productions and each group is a sequence of symbols (TERMINALS/NON_TERMINALS/EPSILON)
 *  and finally eliminate left recursion.
 * @param inputFilePath path to file containing the CFG file of the given language.
 * @return CFG_Reader object containing the rules of the language and the start symbol.
 *
 */
CFG_Reader::CFG_Reader(std::string &inputFilePath) {
    std::fstream file;
    file.open(inputFilePath, std::ios::in);
    if (!file.is_open()) {
        std::cerr << "Invalid CFG file path\n";
        exit(-1);
    }
    has_error = false;
    std::string current_def, line;
    getline(file, current_def);
    while (getline(file, line)) {
        if (line[0] == RULE_START) {
            insert_new_definition(current_def);
            current_def = line;
        } else {
            current_def += line;
        }
    }
    insert_new_definition(current_def);
    file.close();
    eliminate_left_recursion();
}

void CFG_Reader::insert_new_definition(std::string &rule_def) {
    std::istringstream iss(rule_def);
    std::string token, LHS;
    std::vector<std::string> RHS;
    iss >> token;
    if (token.size() != 1 || token[0] != RULE_START) {
        std::cerr << "Rule definitions should start with #\n";
        has_error = true;
        return;
    }
    iss >> LHS >> token;
    if (token.size() != 1 || token[0] != RULE_ASSIGNMENT) {
        std::cerr << "Rule definitions should start with #\n";
        has_error = true;
        return;
    }
    Production current_prod;
    std::vector<Production> productions;
    while (iss >> token) {
        if (token.size() == 1 && token[0] == RULE_SEPARATOR) {
            if (current_prod.empty()) {
                std::cerr << "Production can't be empty\n";
                has_error = true;
                continue;
            }
            productions.push_back(current_prod);
            current_prod.clear();
        } else {
            Symbol symbol;
            if (token[0] == '\'' && token[token.size() - 1] == '\'') {
                symbol = {token.substr(1, token.size() - 2), Type::TERMINAL};
            } else if (token.size() == 2 && token[0] == '\\' && token[1] == 'L') {
                symbol = {token, Type::EPSILON};
            } else {
                symbol = {token, Type::NON_TERMINAL};
            }
            current_prod.push_back(symbol);
        }
    }
    if (current_prod.empty()) {
        std::cerr << "Production can't be empty\n";
        has_error = true;
    } else {
        productions.push_back(current_prod);
    }
    rules[LHS].insert(rules[LHS].end(), productions.begin(), productions.end());
    if (start_symbol.empty()) {
        start_symbol = LHS;
    }
}

void CFG_Reader::eliminate_left_recursion() {

    std::vector<std::string> non_terminals_before;
    for (const auto &rule : rules) {
        non_terminals_before.push_back(rule.first);
    }
    for (auto &i : non_terminals_before) {
        for (auto &j : non_terminals_before) {
            if (j == i) {
                break;
            }
            std::vector<Production> expanded_prod;
            for (auto &prod : rules[i]) {
                if (is_left_dependent(prod, j)) {
                    auto substituted = substitute(prod, rules[j]);
                    expanded_prod.insert(expanded_prod.end(), substituted.begin(), substituted.end());
                } else {
                    expanded_prod.push_back(prod);
                }
            }
            rules[i] = expanded_prod;
        }
        eliminate_immediate_left_recursion(i, rules[i]);
    }
}

void CFG_Reader::eliminate_immediate_left_recursion(const std::string &LHS, const std::vector<Production> &RHS) {
    std::vector<Production> start_with_LHS;
    std::vector<Production> doesnt_start_with_LHS;
    for (const auto &prod : RHS) {
        if (is_left_dependent(prod, LHS)) {
            start_with_LHS.push_back(prod);
        } else {
            doesnt_start_with_LHS.push_back(prod);
        }
    }
    if (start_with_LHS.empty()) {
        return;
    }
    if (doesnt_start_with_LHS.empty()) {
        std::cerr << "Error: Couldn't eliminate left recursion\n";
        has_error = true;
        return;
    }
    rules[LHS].clear();
    for (auto &rule : doesnt_start_with_LHS) {
        rule.push_back({LHS + DASH, Type::NON_TERMINAL});
        rules[LHS].push_back(rule);
    }

    for (auto &rule : start_with_LHS) {
        rule.push_back({LHS + DASH, Type::NON_TERMINAL});
        rule.erase(rule.begin());
        rules[LHS + DASH].push_back(rule);
    }
    rules[LHS + DASH].push_back({{"\\L", Type::EPSILON}});

}

bool CFG_Reader::is_left_dependent(const Production &prod, const std::string &prev_non_terminal) {
    return prod.front().name == prev_non_terminal && prod.front().type == Type::NON_TERMINAL;
}

std::vector<CFG_Reader::Production> CFG_Reader::substitute(Production &curProd, std::vector<Production> prevProd) {
    curProd.erase(curProd.begin());
    for (auto &rule : prevProd) {
        rule.insert(rule.end(), curProd.begin(), curProd.end());
    }
    return prevProd;
}
