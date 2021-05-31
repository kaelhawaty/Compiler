//
// Created by Mahmoudmohamed on 5/30/2021.
//

#include "Syntax_Utils.h"
#include <iostream>
#include <map>
#include <cassert>


// it's just assumption, can be changed later.
const Symbol eps_symbol = {"Є", Type::EPSILON};

Syntax_Utils::Syntax_Utils(const std::unordered_map<std::string, std::vector<Production>> &rules,
                           const Symbol &start_symbol) {

    // First, construct first table.
    for (auto &[non_terminal, _] : rules) {
        Symbol non_terminal_symbol = {non_terminal, Type::NON_TERMINAL};
        this->precompute_first(non_terminal_symbol, rules);
        // initialize the follow sets
        this->follow.insert({non_terminal_symbol, {}});
    }

    // Then, construct Follow table.
    if (this->follow.find(start_symbol) == this->follow.end()) {
        std::cerr << "The start symbol " << start_symbol.name << " does not have a rule.\n";
        this->follow.insert({start_symbol, {}});
    }
    this->precompute_follow(rules, start_symbol);
}

// Assumption it returns the name of the symbol when the symbol is not non-terminal.
Syntax_Utils::First_set Syntax_Utils::first_of(const Symbol &symbol) {
    if (symbol.type == Type::NON_TERMINAL) {
        return Syntax_Utils::find(this->first, symbol);
    }
    else {
        return {symbol};
    }
}

Syntax_Utils::First_set Syntax_Utils::first_of(const Syntax_Utils::Production& production) {
    First_set curr_first;
    for (const Symbol &symbol : production) {
        First_set symbol_first = first_of(symbol);
        curr_first.insert(symbol_first.begin(), symbol_first.end());
    }
    return curr_first;
}

Syntax_Utils::Follow_set Syntax_Utils::follow_of(const Symbol &symbol) {
    assert(symbol.type == Type::NON_TERMINAL);
    return Syntax_Utils::find(this->follow, symbol);
}

Syntax_Utils::Terminal_set
Syntax_Utils::find(std::unordered_map<Symbol, Terminal_set> &set,const Symbol &non_terminal) {
    auto it = set.find(non_terminal);
    if (it == set.end()) {
        return {};
    }
    return it->second;
}

void Syntax_Utils::precompute_first(const Symbol &non_terminal,
                                    const std::unordered_map<std::string, std::vector<Production>> &rules) {

    // Checks if we have visited that non-terminal before.
    if (this->first.find(non_terminal) != this->first.end()) {
        return;
    }
    this->first.insert({non_terminal, {}});

    // this type of error could be handled in different ways
    // but for now, we will insert a new rule
    // "undefined_non_terminal --> Є"
    // which makes the first set {Є}.
    if (rules.find(non_terminal.name) == rules.end()) {
        std::cerr << non_terminal.name << " is not defined.\n";
        this->first.at(non_terminal).insert(eps_symbol);
        // Also initialize its follow set, since it won't be found in the rules map.
        this->follow.insert({non_terminal, {}});
        return;
    }
    // Keeps track if the first set of the current non-terminal so far.
    First_set &current_first = this->first.at(non_terminal);

    // As long as epsilon is false, the final first set shouldn't contain epsilon.
    bool add_epsilon = false;
    for (const Production &production : rules.at(non_terminal.name)) {

        // epsilon flag relative to the current production.
        // it's used also while iterating to decide if we should stop iterating if
        // we encounter a terminal or a non-terminal with a free set without epsilon.
        bool curr_epsilon = true;
        for (int i = 0; i < production.size() && curr_epsilon; i++) {
            const Symbol &symbol = production.at(i);
            if (symbol.type == Type::TERMINAL) {
                current_first.insert(symbol);
                curr_epsilon = false;
            } else if (symbol.type == Type::NON_TERMINAL) {
                precompute_first(symbol, rules);
                const First_set &temp = this->first.at(symbol);
                if (temp.find(eps_symbol) == temp.end()) {
                    curr_epsilon = false;
                }
                current_first.insert(temp.begin(), temp.end());
            }
            else if (symbol.type == Type::EPSILON) {
                current_first.emplace(eps_symbol);
            }
        }
        add_epsilon |= curr_epsilon;
    }

    bool eps_in_first = current_first.find(eps_symbol) != current_first.end();

    // Makes sure if the epsilon should be in the first set or not.
    if (eps_in_first && !add_epsilon) {
        current_first.erase(eps_symbol);
    } else if (!eps_in_first && add_epsilon) {
        current_first.insert(eps_symbol);
    }
}

void Syntax_Utils::precompute_follow(const std::unordered_map<std::string, std::vector<Production>> &rules,
                                     const Symbol &start_symbol) {
    this->follow.at(start_symbol).insert({"$", Type::TERMINAL});
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
void Syntax_Utils::follow_calculate_by_first(const std::unordered_map<std::string, std::vector<Production>> &rules) {

    for (const auto &[lhs_non_terminal, rule] : rules) {
        for (const Production &production: rule) {
            First_set curr_first;
            for (int i = (int)production.size() - 1; i >= 0; i--) {
                const Symbol &symbol = production.at(i);
                if (symbol.type == Type::NON_TERMINAL) {
                    // Update the follow set with the first set of the suffix non-terminals.
                    this->follow.at(symbol).insert(curr_first.begin(), curr_first.end());
                    auto symbol_first = this->first.at(symbol);
                    if (symbol_first.find(eps_symbol) == symbol_first.end()) {
                        // reset the first set of the suffix non-terminals.
                        curr_first.clear();
                    }
                    else {
                        symbol_first.erase(eps_symbol);
                    }
                    curr_first.insert(symbol_first.begin(), symbol_first.end());

                }
                else if (symbol.type == Type::TERMINAL){
                    curr_first.clear();
                    curr_first.insert(symbol);
                }
            }
        }
    }
}

/*
 *  It keeps applying the third rule until no change is observed in the follow sets.
 *  Changes are observed by keeping tracks of the size of each follow set and compare the old with the new one.
 */
void Syntax_Utils::follow_calculate_by_follow(const std::unordered_map<std::string, std::vector<Production>> &rules) {

    bool updated;
    do {
        updated = false;
        for (const auto &[lhs_non_terminal, rule] : rules) {
            const auto &lhs_follow = this->follow.at({lhs_non_terminal, Type::NON_TERMINAL});
            for (const Production &production: rule) {
                for (int i = (int)production.size() - 1; i >= 0 && production.at(i).type == Type::NON_TERMINAL; i--) {
                    const Symbol &symbol = production.at(i);
                    const auto &symbol_first = this->first.at(symbol);
                    auto &symbol_follow = this->follow.at(symbol);
                    updated |= Syntax_Utils::insert_into(symbol_follow, lhs_follow);
                    if (symbol_first.find(eps_symbol) == symbol_first.end()) {
                        break;
                    }
                }
            }
        }
    } while (updated);
}

bool Syntax_Utils::insert_into(Syntax_Utils::Terminal_set &dest, const Syntax_Utils::Terminal_set &src) {
    int size = dest.size();
    dest.insert(src.begin(), src.end());
    return dest.size() != size;
}
