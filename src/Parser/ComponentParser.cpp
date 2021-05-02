//
// Created by Mahmoudmohamed on 4/29/2021.
//

#include <stack>
#include <iostream>
#include "ComponentParser.h"

using namespace std;


/**
 * Gets the top element of a stack pop it from a stack.
 */
template<typename t>
t poll(std::stack<t>& st) {
    if (!st.empty()) {
        t top = std::move(st.top());
        st.pop();
        return top;
    }
    throw logic_error("Stack is empty, internal Error");
}


/**
 * compare the two component types based on their Precedence.
 */
bool hasPrecedence(component_type t1, component_type t2) {
    return (t1 == CONCAT && t2 == OR);
}


const std::unordered_map<std::string, NFA>& ComponentParser::regDefinitionsToNFAs(const vector<std::pair<std::string, std::vector<component>>> & regDefinitions) {
    for (const auto& [regularDefinition, components] : regDefinitions) {
        // If the case is a single char.
        try {
            if (components.size() == 1 && components[0].regularDefinition.size() == 1)
                this->regToNFA[regularDefinition] = ComponentParser::CharToNFA(components[0]);
            else
                this->regToNFA[regularDefinition] = this->SingleRegDefToNFA(components);
        } catch (logic_error& e) {
            std::cerr << "Couldn't Parse " << regularDefinition << "\n" << e.what() << "\nCheck your rules format.";
        }
    }
    return this->regToNFA;
}

NFA ComponentParser::CharToNFA(const component& regDefinitionChar) {
    return NFA(regDefinitionChar.regularDefinition[0]);
}

NFA ComponentParser::SingleRegDefToNFA(const std::vector<component>& components) {

    std::stack<NFA_Builder> NFABuilders;
    std::stack<component_type> operations;

    for (int i = 0; i < components.size(); i++) {
        component comp = components[i];
        // 'TO' has highest precedence if found.
        if (i + 1 < components.size() && components[i + 1].type == TO) {
            if (i + 2 >= components.size()) {
                throw logic_error("to Use '-', you have to have two chars around it");
            }
            NFABuilders.push(applyToOperation(components[i], components[i + 2]));
            i += 2;
        }
        else if (comp.type == POS_CLOSURE || comp.type == KLEENE_CLOSURE) {
            NFABuilders.push(addClosure(comp, poll(NFABuilders)));
        }
        // If open Brackets was found, then get the whole expression within the brackets as a single NFA_builder.
        else if (comp.type == OPEN_BRACKETS) {
            NFABuilders.push(addExpressionInBrackets(components, &i));
        }
        else if (comp.type == REG_EXP || comp.type == RED_DEF) {
            if (regToNFA.find(comp.regularDefinition) == regToNFA.end())
                throw logic_error(comp.regularDefinition + " Was not seen before.");
            NFABuilders.push(NFA_Builder(regToNFA[comp.regularDefinition]));
        }
        // Apply a binary operation based on the precedence.
        else if (comp.type == CONCAT || comp.type == OR) {
            while (!operations.empty() && hasPrecedence(operations.top(), comp.type)) {
                NFABuilders.push(applyBinaryOperation(poll(operations), poll(NFABuilders), poll(NFABuilders)));
            }
            operations.push(comp.type);
        }
    }

    // Check if there's more operations.
    while (!operations.empty()) {
        NFABuilders.push(applyBinaryOperation(poll(operations), poll(NFABuilders), poll(NFABuilders)));
    }

    return NFABuilders.top().build();
}


NFA_Builder ComponentParser::addExpressionInBrackets(const vector<component>& components, int *index) {
    std::vector<component> temp;
    int bracketsCount = 1;
    int j;
    // Get the whole Expression between brackets.
    for (j = *index + 1; j < components.size(); j++) {
        if (components[j].type == OPEN_BRACKETS)
            bracketsCount++;
        if (components[j].type == CLOSE_BRACKETS)
            bracketsCount--;
        if (bracketsCount == 0) {
            break;
        }
        else if (bracketsCount < 0) {
            // bad parsed
            throw logic_error("Brackets are not balanced");
        }
        temp.push_back(components[j]);
    }
    // If we can't find the corresponding closing brackets.
    if (bracketsCount != 0) {
        // bad parsing
        throw logic_error("Brackets are not balanced");
    }
    *index = j;
    return NFA_Builder(SingleRegDefToNFA(temp));
}

NFA_Builder ComponentParser::addClosure(const component& comp, NFA_Builder nfaBuilder) {
    return std::move(comp.type == POS_CLOSURE ? nfaBuilder.Positive_closure() : nfaBuilder.Kleene_closure());
}

NFA_Builder ComponentParser::applyBinaryOperation(const component_type type, NFA_Builder f, NFA_Builder s) {
    switch (type) {
        case CONCAT: return std::move(f.Concatenate(s.build()));
        case OR: return std::move(f.Or(s.build()));
        default: throw logic_error("Unknown Operation found");
    }
}

NFA_Builder ComponentParser::applyToOperation(const component& c1, const component& c2) {
    // Assumption that 'TO' operation takes only rhs char, and lhs char.
    if (c1.regularDefinition.size() != 1 && c2.regularDefinition.size() != 1) {
        throw logic_error("Check '-' syntax, Just use one char at each end\"eg: a-z\"");
    }
    char first = c1.regularDefinition[0];
    char second = c2.regularDefinition[0];
    if (second - first < 0) throw logic_error("Check '-' syntax, eg: You can use a-z not z-a");
    NFA_Builder nfa_builder((NFA(first)));
    while (++first <= second) {
        nfa_builder.Or(NFA (first));
    }
    return std::move(nfa_builder);
}


