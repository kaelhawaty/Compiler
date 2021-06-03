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

void Rules_builder::apply_left_factoring() {
    std::unordered_map<Symbol, Rule> new_rules;
    for(const auto &rule : this->rules){
        const std::unordered_map<Symbol, Rule> &factored_rules = left_factor_rule(rule.first, rule.second);
        new_rules.insert(factored_rules.begin(),factored_rules.end());
    }
    this->rules = new_rules;
}

std::unordered_map<Symbol, Rule> Rules_builder::left_factor_rule(const Symbol &lhs, const Rule &rule) {
    std::shared_ptr<Node> root = std::make_shared<Node> ();
    for(const Production &production : rule)
        addSymbols(root,production,0);

    std::unordered_map<Symbol, Rule> new_rules;
    if(root->children.size() == 1){
        new_rules.insert({lhs,{}});
        Production new_production = dfs(root->children.begin()->second,new_rules,lhs);
        new_production.insert(new_production.begin(),root->children.begin()->first);
        remove_unnecessary_epsilon(new_production);
        new_rules[lhs] = {move(new_production)};
    }else{
        dfs(root,new_rules,lhs);
    }
    return new_rules;
}

void Rules_builder::addSymbols(const std::shared_ptr<Node> &node, const std::vector<Symbol> &symbols, int symbolIndex) {
    if(symbols.size() == symbolIndex){
        node->children.insert({eps_symbol, std::make_shared<Node>()});
        return;
    }

    if(node->children.find(symbols[symbolIndex]) == node->children.end())
        node->children.insert({symbols[symbolIndex], std::make_shared<Node>()});
    addSymbols(node->children[symbols[symbolIndex]], symbols, symbolIndex+1);
}

std::vector<Symbol> Rules_builder::dfs(const std::shared_ptr<Node> &node, std::unordered_map<Symbol, Rule> &new_rules,
                                       const Symbol &origin_lhs) {
    if(node->children.empty())
        return {};

    if(node->children.size() == 1){
        std::vector<Symbol> symbols = dfs(node->children.begin()->second,new_rules,origin_lhs);
        symbols.insert(symbols.begin(),node->children.begin()->first);
        return symbols;
    }

    Symbol new_lhs = origin_lhs;
    if(new_rules.size() >= 1)
        new_lhs.name += std::to_string(new_rules.size());
    new_rules.insert({new_lhs,{}});

    Rule new_rule;
    for(auto &[symbol,child]:node->children){
        Production new_production = dfs(child,new_rules,origin_lhs);
        new_production.insert(new_production.begin(),symbol);
        remove_unnecessary_epsilon(new_production);
        new_rule.emplace_back(move(new_production));
    }
    new_rules[new_lhs] = move(new_rule);
    return {new_lhs};
}

void Rules_builder::remove_unnecessary_epsilon(Production &production) {
    if(production.size() >= 2 && production.back() == eps_symbol)
        production.pop_back();
}




