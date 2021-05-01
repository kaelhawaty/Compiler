//
// Created by Mahmoudmohamed on 4/29/2021.
//

#include <stack>
#include "ComponentParser.h"

using namespace std;


/**
 * Gets the top element of a stack pop it from a stack.
 */
template<typename t>
t poll(std::stack<t>& st) {
    if (!st.empty()) {
        t top = st.top();
        st.pop();
        return top;
    }
    exit(-1);
}


/**
 * compare the two component types based on their Precedence.
 */
bool hasPrecedence(component_type t1, component_type t2) {
    return (t1 == CONCAT && t2 == OR);
}


std::unordered_map<std::string, NFA> ComponentParser::regDefinitionsToNFAs(vector<std::pair<std::string, std::vector<component>>> & regDefinitions) {
    for (std::pair<std::string, std::vector<component>> regDef: regDefinitions) {
        // If the case is a single char.
        if (regDef.second.size() == 1)
            this->regToNFA[regDef.first] = ComponentParser::CharToNFA(regDef.second[0]);
        else
            this->regToNFA[regDef.first] = this->SingleRegDefToNFA(regDef.second);
    }
    return this->regToNFA;
}

NFA ComponentParser::CharToNFA(component& regDefinitionChar) {
    return NFA(regDefinitionChar.regularDefinition[0]);
}

NFA ComponentParser::SingleRegDefToNFA(std::vector<component>& components) {

    std::stack<NFA_Builder> NFABuilders;
    std::stack<component_type> operations;

    for (int i = 0; i < components.size(); i++) {
        component comp = components[i];
        // 'TO' has highest precedence if found.
        if (i + 1 < components.size() && components[i + 1].type == TO) {
            if (i + 2 >= components.size()) {
                exit(-1);
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


NFA_Builder ComponentParser::addExpressionInBrackets(vector<component>& components, int *index) {
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
            exit(-1);
        }
        temp.push_back(components[j]);
    }
    // If we can't find the corresponding closing brackets.
    if (bracketsCount != 0) {
        // bad parsing
        exit(-1);
    }
    *index = j;
    return NFA_Builder(SingleRegDefToNFA(temp));
}

NFA_Builder ComponentParser::addClosure(component& comp, NFA_Builder&& nfaBuilder) {
    return comp.type == POS_CLOSURE ? nfaBuilder.Positive_closure() : nfaBuilder.Kleene_closure();
}

NFA_Builder ComponentParser::applyBinaryOperation(component_type type, NFA_Builder&& firstNFABuilder, NFA_Builder&& secondNFABuilder) {
    switch (type) {
        case CONCAT: return firstNFABuilder.Concatenate(secondNFABuilder.build());
        case OR: return firstNFABuilder.Or(secondNFABuilder.build());
        default: exit(-1);
    }
}

NFA_Builder ComponentParser::applyToOperation(component& c1, component& c2) {
    // Assumption that 'TO' operation takes only rhs char, and lhs char.
    char first = c1.regularDefinition[0];
    char second = c2.regularDefinition[0];
    if (second - first < 0) exit(-1);
    NFA_Builder nfa_builder((NFA(first)));
    while (++first <= second) {
        nfa_builder.Or(NFA (first));
    }
    return nfa_builder;
}


