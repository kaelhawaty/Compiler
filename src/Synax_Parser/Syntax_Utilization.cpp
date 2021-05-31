//
// Created by Mahmoudmohamed on 5/30/2021.
//

#include "Syntax_Utilization.h"
#include <iostream>
#include <map>

// it's just assumption, can be changed later.
const static string eps_str = "Є";

Syntax_Utilization::Syntax_Utilization(const unordered_map<string, vector<vector<Symbol>>> &rules,
                                       const string &start_symbol) {

    // First, construct first table.
    // To keep track of the visited non-terminals
    unordered_set<string> seen_non_terminals;
    for (auto &[non_terminal, _] : rules) {
        this->first_calculate(non_terminal, rules, seen_non_terminals);
        // initialize the follow sets
        this->follow.insert({non_terminal, {}});
    }

    // Then, construct Follow table.
    if (this->follow.find(start_symbol) == this->follow.end()) {
        cerr << "The start symbol " << start_symbol << " does not have a rule.\n";
        this->follow[start_symbol] = {};
    }
    this->follow_calculate(rules, start_symbol);
}

bool Syntax_Utilization::first_of(const std::string &non_terminal, unordered_set<std::string> &first_of_non_terminal) {
    return this->find(this->first, non_terminal, first_of_non_terminal);
}

bool Syntax_Utilization::follow_of(const string &non_terminal, unordered_set<string> &follow_of_non_terminal) {
    return this->find(this->follow, non_terminal, follow_of_non_terminal);
}

bool Syntax_Utilization::find(unordered_map<string, unordered_set<string>> &set,
                              const string &non_terminal,
                              unordered_set<string> &set_of_non_terminal) {
    auto it = set.find(non_terminal);
    if (it == set.end()) {
        set_of_non_terminal = {non_terminal};
        return false;
    }
    set_of_non_terminal = it->second;
    return true;
}

void Syntax_Utilization::first_calculate(const string &non_terminal,
                                         const unordered_map<string, vector<vector<Symbol>>> &rules,
                                         unordered_set<string> &seen_non_terminals) {
    // Checks if we have visited that non-terminal before.
    if (seen_non_terminals.find(non_terminal) != seen_non_terminals.end()) {
        return;
    }
    seen_non_terminals.emplace(non_terminal);
    // this type of error could be handled in different ways
    // but for now, we will insert a new rule
    // "undefined_non_terminal --> Є"
    // which makes the first set {Є}.
    if (rules.find(non_terminal) == rules.end()) {
        cerr << non_terminal << " is not defined.\n";
        this->first.insert({non_terminal, {eps_str}});
        // Also initialize its follow set, since it won't be found in the rules map.
        this->follow.insert({non_terminal, {}});
        return;
    }
    // Keeps track if the first set of the current non-terminal so far.
    unordered_set<string> current_first;
    // As long as epsilon is false, the final first set shouldn't contain epsilon.
    bool epsilon = false;
    for (const vector<Symbol> &production : rules.at(non_terminal)) {
        // epsilon flag relative to the current production.
        // it's used also while iterating to decide if we should stop iterating if
        // we encounter a terminal or a non-terminal with a free set without epsilon.
        bool curr_epsilon = true;
        for (int i = 0; i < production.size() && curr_epsilon; i++) {
            const Symbol &symbol = production.at(i);
            if (symbol.type == Type::TERMINAL) {
                current_first.insert(symbol.name);
                curr_epsilon = false;
            } else if (symbol.type == Type::NON_TERMINAL) {
                first_calculate(symbol.name, rules, seen_non_terminals);
                const unordered_set<string> &temp = this->first.at(symbol.name);
                if (temp.find(eps_str) == temp.end()) {
                    curr_epsilon = false;
                }
                current_first.insert(temp.begin(), temp.end());
            }
            else {
                current_first.emplace(eps_str);
            }
        }
        epsilon |= curr_epsilon;
    }
    // Makes sure if the epsilon should be in the first set or not.
    if (current_first.find(eps_str) != current_first.end()) {
        if (!epsilon)
            current_first.erase(eps_str);
    } else if (epsilon) {
        current_first.insert(eps_str);
    }

    this->first.insert({non_terminal, current_first});
}

void Syntax_Utilization::follow_calculate(const unordered_map<string, vector<vector<Symbol>>> &rules,
                                          const string &start_symbol) {
    this->follow.at(start_symbol).insert("$");
    this->follow_calculate_by_first(rules);
    this->follow_calculate_by_follow(rules);
}

/*
 *    Performing the calculation of first sets incrementally.
 *    So for example: A => B C D and let's assume D and C have Є
 *    then to calculate FOLLOW(B), we will need to merge FIRST(C) and FIRST(D)
 *    and to calculate FOLLOW(C), we will need to get FIRST(D).
 *    so we can simply starts iterating backward and just merge if the non-terminal has Є.
 */
void Syntax_Utilization::follow_calculate_by_first(const unordered_map<string, vector<vector<Symbol>>> &rules) {

    for (const auto &[lhs_non_terminal, rule] : rules) {
        for (const vector<Symbol> &production: rule) {
            unordered_set<string> curr_first;
            for (int i = production.size() - 1; i >= 0; i--) {
                const Symbol &symbol = production.at(i);
                if (symbol.type == Type::NON_TERMINAL) {
                    // Update the follow set with the first set of the suffix non-terminals.
                    this->follow.at(symbol.name).insert(curr_first.begin(), curr_first.end());
                    auto symbol_first = this->first.at(symbol.name);
                    if (symbol_first.find(eps_str) == symbol_first.end()) {
                        // reset the first set of the suffix non-terminals.
                        curr_first = {};
                    }
                    else {
                        symbol_first.erase(eps_str);
                    }
                    curr_first.insert(symbol_first.begin(), symbol_first.end());

                }
                else if (symbol.type == Type::TERMINAL){
                    curr_first = {symbol.name};
                }
            }
        }
    }
}

/*
 *  It keeps applying the third rule until no change is observed in the follow sets.
 *  Changes are observed by keeping tracks of the size of each follow set and compare the old with the new one.
 */
void Syntax_Utilization::follow_calculate_by_follow(const unordered_map<string, vector<vector<Symbol>>> &rules) {

    std::map<string, int> sizes_of_follows;

    for (const auto &[lhs_non_terminal, _] : rules) {
        sizes_of_follows.emplace(lhs_non_terminal, this->follow.at(lhs_non_terminal).size());
    }

    std::map<string, int> sizes_update(sizes_of_follows);
    do {
        for (const auto &[lhs_non_terminal, rule] : rules) {
            for (const vector<Symbol> &production: rule) {
                for (int i = production.size() - 1; i >= 0; i--) {
                    const Symbol &symbol = production.at(i);
                    if (symbol.type == Type::NON_TERMINAL) {
                        const auto &symbol_first = this->first.at(symbol.name);
                        const auto &lhs_follow = this->follow.at(lhs_non_terminal);
                        auto &symbol_follow = this->follow.at(symbol.name);
                        symbol_follow.insert(lhs_follow.begin(), lhs_follow.end());
                        sizes_update[symbol.name] = symbol_follow.size();
                        if (symbol_first.find(eps_str) == symbol_first.end()) {
                            break;
                        }
                    }
                    else
                        break;
                }
            }
        }
        swap(sizes_of_follows, sizes_update);
    } while (sizes_of_follows != sizes_update);
}
