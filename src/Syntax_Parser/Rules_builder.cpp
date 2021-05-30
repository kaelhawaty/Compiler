//
// Created by hazem on 5/30/2021.
//

#include "Rules_builder.h"

const char PRODUCTION_SEPARATOR = '|';
const char RULE_START = '#';
const char RULE_ASSIGNMENT = '=';
const char DASH = '\'';

/**
 *  Reads the CFG rules file line by line and parse each rule definition to its groups
 *  of productions and each group is a sequence of symbols (TERMINALS/NON_TERMINALS/EPSILON)
 *  and finally eliminate left recursion.
 * @param inputFilePath path to file containing the CFG file of the given language.
 */
Rules_builder::Rules_builder(const std::string &inputFilePath) : has_error(false) {
    std::unordered_map<std::string, bool> defined_in_lhs;
    std::fstream file;
    file.open(inputFilePath, std::ios::in);
    if (!file.is_open()) {
        std::cerr << "Invalid CFG file path\n";
        exit(-1);
    }
    std::string current_def, line;
    getline(file, current_def);
    while (getline(file, line)) {
        if (line[0] == RULE_START) {
            insert_new_definition(current_def, defined_in_lhs);
            current_def = line;
        } else {
            current_def += ' ' + line;
        }
    }
    insert_new_definition(current_def, defined_in_lhs);
    file.close();
    for (const auto &[symbol, defined] : defined_in_lhs) {
        if (!defined) {
            std::cerr << "Symbol: " << symbol << " is not defined but used!\n";
            has_error = true;
        }
    }
}

void
Rules_builder::insert_new_definition(std::string &rule_def, std::unordered_map<std::string, bool> &defined_in_lhs) {
    std::istringstream iss(rule_def);
    std::string token, LHS;
    iss >> token;
    if (token.size() != 1 || token[0] != RULE_START) {
        std::cerr << "Rule definitions should start with #\n";
        has_error = true;
        return;
    }
    iss >> LHS >> token;
    if (token.size() != 1 || token[0] != RULE_ASSIGNMENT) {
        std::cerr << "Rule definitions should contain = after LHS.\n";
        has_error = true;
        return;
    }
    defined_in_lhs[LHS] = true;
    Production current_prod;
    std::vector<Production> productions;
    while (iss >> token) {
        if (token.size() == 1 && token[0] == PRODUCTION_SEPARATOR) {
            if (current_prod.empty()) {
                std::cerr << "Productions can't be empty\n";
                has_error = true;
                continue;
            }
            productions.push_back(std::move(current_prod)), current_prod = Production{};
        } else {
            Symbol symbol;
            if (token[0] == '\'' && token[token.size() - 1] == '\'') {
                symbol = {token.substr(1, token.size() - 2), Symbol::Type::TERMINAL};
            } else if (token.size() == 2 && token[0] == '\\' && token[1] == 'L') {
                symbol = eps_symbol;
            } else {
                symbol = {token, Symbol::Type::NON_TERMINAL};
                if (!defined_in_lhs.count(token)) {
                    defined_in_lhs.insert({token, false});
                }
            }
            current_prod.push_back(std::move(symbol));
        }
    }
    if (current_prod.empty()) {
        std::cerr << "Productions can't be empty\n";
        has_error = true;
    } else {
        productions.push_back(std::move(current_prod));
    }

    Rule &rule_lhs = rules[{LHS, Symbol::Type::NON_TERMINAL}];
    rule_lhs.insert(rule_lhs.end(), productions.begin(), productions.end());
    if (start_symbol.name.empty()) {
        start_symbol = {LHS, Symbol::Type::NON_TERMINAL};
    }
}

const std::unordered_map<Symbol, Rule> &Rules_builder::getRules() const {
    return this->rules;
}

const Symbol &Rules_builder::getStartSymbol() const {
    return this->start_symbol;
}

bool Rules_builder::fail() const {
    return this->has_error;
}

void Rules_builder::eliminate_left_recursion() {
    std::vector<std::vector<Symbol>> new_rules;
    for (auto &i : rules) {
        for (auto &j : rules) {
            if (j == i) {
                break;
            }
            std::vector<std::vector<Symbol>> expanded_prod;
            for (auto &prod : i.second) {
                if (is_left_dependent(prod, j.first.name)) {
                    auto substituted = substitute(prod, j.second);
                    expanded_prod.insert(expanded_prod.end(), substituted.begin(), substituted.end());
                }else {
                    expanded_prod.push_back(prod);
                }
            }
            i.second = expanded_prod;
        }
        eliminate_immediate_left_recursion(i.first.name, i.second, new_rules);
    }
}

void Rules_builder::eliminate_immediate_left_recursion(const std::string &LHS, const std::vector<std::vector<Symbol>> &RHS, std::vector<std::vector<Symbol>> &new_rules) {
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

bool Rules_builder::is_left_dependent(const std::vector<Symbol> &prod, const std::string &prev_non_terminal) {
    return prod.front().name == prev_non_terminal && prod.front().type == Symbol::Type::NON_TERMINAL;
}

std::vector<std::vector<Symbol>> Rules_builder::substitute(std::vector<Symbol> &curProd, std::vector<std::vector<Symbol>> prevProd) {
    curProd.erase(curProd.begin());
    for (auto &rule : prevProd) {
        rule.insert(rule.end(), curProd.begin(), curProd.end());
    }
    return prevProd;
}