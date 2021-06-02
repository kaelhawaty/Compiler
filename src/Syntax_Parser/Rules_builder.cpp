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
    eliminate_left_recursion();
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

    std::vector<Symbol> non_terminals_before;
    for (const auto &[symbol, rule] : rules) {
        non_terminals_before.push_back(symbol);
    }
    for (const auto &current : non_terminals_before) {
        for (const auto &previous : non_terminals_before) {
            if (current == previous) {
                break;
            }
            Rule expanded_rule;
            for (auto &prod : rules[current]) {
                if (is_left_dependent(prod, previous)) {
                    auto substituted = substitute(prod, rules[previous]);
                    expanded_rule.insert(expanded_rule.end(), substituted.begin(), substituted.end());
                } else {
                    expanded_rule.push_back(prod);
                }
            }
            rules[current] = expanded_rule;
        }
        eliminate_immediate_left_recursion(current, rules[current]);
    }
}

void Rules_builder::eliminate_immediate_left_recursion(const Symbol &LHS, const std::vector<Production> &RHS) {
    Rule start_with_LHS;
    Rule doesnt_start_with_LHS;
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
        exit(-1);
    }
    auto &this_rule = rules[LHS];
    this_rule.clear();
    Symbol new_LHS = {LHS.name+DASH, Symbol::Type::NON_TERMINAL};
    for (auto &prod : doesnt_start_with_LHS) {
        prod.push_back(new_LHS);
        this_rule.push_back(prod);
    }
    auto &new_rule = rules[new_LHS];
    for (auto &rule : start_with_LHS) {
        rule.push_back(new_LHS);
        rule.erase(rule.begin());
        new_rule.push_back(rule);
    }
    new_rule.push_back({eps_symbol});
}

bool Rules_builder::is_left_dependent(const Production &prod, const Symbol &prev_non_terminal) {
    return prod.front() == prev_non_terminal;
}

std::vector<Production> Rules_builder::substitute(Production &curProd, std::vector<Production> prevProd) {
    curProd.erase(curProd.begin());
    for (auto &rule : prevProd) {
        rule.insert(rule.end(), curProd.begin(), curProd.end());
    }
    return prevProd;
}