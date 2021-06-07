//
// Created by hazem on 5/30/2021.
//

#include <algorithm>
#include <cassert>
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

void Rules_builder::buildLL1Grammar() {
    eliminate_left_recursion();
    apply_left_factoring();
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

    Symbol lhs_sym{LHS, Symbol::Type::NON_TERMINAL};
    Rule &rule_lhs = rules.insert({lhs_sym, Rule(lhs_sym.name)}).first->second;

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
/*
 * left factoring is applied on each Rule independently.
 * Each Rule is converted to multiple rules that represent the new rules.
 */
void Rules_builder::apply_left_factoring() {
    std::unordered_map<Symbol, Rule> new_rules;
    for(const auto &rule : this->rules){
        const std::unordered_map<Symbol, Rule> &factored_rules = left_factor_rule(rule.first, rule.second);
        new_rules.insert(factored_rules.begin(),factored_rules.end());
    }
    this->rules = std::move(new_rules);
}
/*
 * To factor a single Rule.
 * 1- Build a trie for all productions present in the Rule.
 * 2- Traverse the trie and add new rules when needed.
 */
std::unordered_map<Symbol, Rule> Rules_builder::left_factor_rule(const Symbol &lhs, const Rule &rule) {
    std::unique_ptr<Node> root = std::make_unique<Node> ();
    // building the trie by adding each production symbols to it.
    for(const Production &production : rule)
        addProduction(root.get(), production);

    //this will store the new rules generated from factoring given rule
    std::unordered_map<Symbol, Rule> new_rules;
    if(root->children.size() == 1){
        // This a special case when the root contains one child.
        // We must add the first rule here from the return value from dfs call.
        new_rules.insert({lhs,Rule(lhs.name)});
        Production new_production = dfs(root.get(),new_rules,lhs);
        reformat_production(new_production);
        new_rules[lhs] = Rule(lhs.name,{std::move(new_production)});
    }else{
        dfs(root.get(),new_rules,lhs);
    }
    return new_rules;
}

// add a production to the trie by adding its symbols to corresponding nodes.
void Rules_builder::addProduction(Node* node, const Production &production) {
    for(const auto &symbol : production){
        if(node->children.find(symbol) == node->children.end())
            node->children.insert({symbol, std::make_unique<Node>()});
        node = node->children[symbol].get();
    }
    // add epsilon at the end of every production to mark leaf nodes.
    // handle the case when one production is prefix of another.
    node->children.insert({eps_symbol, std::make_unique<Node>()});
}
// dfs the whole trie and add a rule for every node with more than one child.
std::vector<Symbol> Rules_builder::dfs(Node* node, std::unordered_map<Symbol, Rule> &new_rules,
                                       const Symbol &origin_lhs) {
    // base case when reaching leaf node.
    if(node->children.empty())
        return {};

    // if node has only one child no need to add new rule.
    // Just continue the dfs and the child to be a prefix to the returned value when reversed.
    if(node->children.size() == 1){
        const auto& onlyChild = node->children.begin();
        std::vector<Symbol> symbols = dfs(onlyChild->second.get(),new_rules,origin_lhs);
        symbols.push_back(onlyChild->first);
        return symbols;
    }

    // if more than one child is present, we need to add a rule where each child will have a production in this rule.
    Symbol new_lhs = origin_lhs;
    //The new Rule lhs is determined from the new_rules current size to have a unique lhs for each rule (A, A1, A2, ...)
    if(new_rules.size() >= 1)
        new_lhs.name += std::to_string(new_rules.size());
    new_rules.insert({new_lhs,Rule(new_lhs.name)});


    Rule new_rule = Rule(new_lhs.name);
    for(const auto &[symbol,child]:node->children){
        // Get the rest of the production for this child.
        Production new_production = dfs(child.get(),new_rules,origin_lhs);
        // Add child symbol to be a prefix for the production when reversed.
        new_production.push_back(symbol);
        reformat_production(new_production);
        new_rule.emplace_back(std::move(new_production));
    }
    new_rules[new_lhs] = Rule(new_lhs.name,std::move(new_rule));
    return {new_lhs};
}

// Reverse the production and Remove the epsilon added to mark leaf nodes if it unnecessary.
// It is kept when one production is prefix of another
// Example : A -> a | ab
// After   : A -> aA1
//          A1 -> b | Є
void Rules_builder::reformat_production(Production &production) {
    std::reverse(production.begin(),production.end());
    if(production.size() >= 2 && production.back() == eps_symbol)
        production.pop_back();
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
            Rule expanded_rule(current.name);
            for (auto &prod : rules.at(current)) {
                if (is_left_dependent(prod, previous)) {
                    auto substituted = substitute(prod, rules.at(previous));
                    expanded_rule.insert(expanded_rule.end(), substituted.begin(), substituted.end());
                } else {
                    expanded_rule.push_back(prod);
                }
            }
            rules.at(current) = expanded_rule;
        }
        eliminate_immediate_left_recursion(rules.at(current));
    }
}

void Rules_builder::eliminate_immediate_left_recursion(Rule &rule) {
    Rule start_with_LHS{""};
    Rule doesnt_start_with_LHS{""};
    for (const auto &prod : rule) {
        if (is_left_dependent(prod, rule.get_lhs())) {
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
    rule.clear();
    Symbol new_LHS = {rule.get_lhs().name+DASH, Symbol::Type::NON_TERMINAL};
    for (auto &prod : doesnt_start_with_LHS) {
        prod.push_back(new_LHS);
        rule.push_back(prod);
    }
    Rule new_rule{new_LHS.name};
    for (auto &prod : start_with_LHS) {
        prod.push_back(new_LHS);
        prod.erase(prod.begin());
        new_rule.push_back(prod);
    }
    new_rule.push_back({eps_symbol});
    rules.insert({new_LHS, new_rule});
}

bool Rules_builder::is_left_dependent(const Production &prod, const Symbol &prev_non_terminal) {
    return prod.front() == prev_non_terminal;
}

Rule Rules_builder::substitute(Production &curProd, Rule prevRule) {
    curProd.erase(curProd.begin());
    for (auto &rule : prevRule) {
        rule.insert(rule.end(), curProd.begin(), curProd.end());
    }
    return prevRule;
}

