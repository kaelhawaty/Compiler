//
// Created by Mahmoudmohamed on 4/29/2021.
//

#include <stack>
#include "ComponentParser.h"

template<typename t>
t poll(std::stack<t>& st) {
    if (!st.empty()) {
        t top = st.top();
        st.pop();
        return top;
    }
    exit(-1);
}
// it can be extendable to more operations
bool hasPrecedence(component_type t1, component_type t2) {
    return t1 == OR || (t1 == CONCAT && t2 == OR);
}

NFA ComponentParser::buildParseTree(std::vector<component>& components) {
    std::stack<NFA> NFAs;
    std::stack<component_type> operations;
    for (int i = 0; i < components.size(); i++) {
        component comp = components[i];
        if (i + 1 < components.size() && components[i + 1].type == TO) {
            if (i + 2 >= components.size()) {
                exit(-1);
            }
            NFAs.push(applyToOperation(components[i], components[i + 2]));
            i += 2;
        }
        else if (comp.type == POS_CLOSURE || comp.type == KLEENE_CLOSURE) {
            NFAs.push(addClosure(comp, poll(NFAs)));
        }
        else if (comp.type == OPEN_BRACKETS) {
            NFAs.push(addExpressionInBrackets(components, &i));
        }
        else if (comp.type == REG_EXP || comp.type == RED_DEF) {
            NFAs.push(regToNFA[comp.regularDefinition]);
        }
        else if (comp.type == CONCAT || comp.type == OR) {
            while (!operations.empty() && hasPrecedence(operations.top(), comp.type)) {
                NFAs.push(applyBinaryOperation(poll(operations), poll(NFAs), poll(NFAs)));
            }
            operations.push(comp.type);
        }
    }

    while (!operations.empty()) {
        NFAs.push(applyBinaryOperation(poll(operations), poll(NFAs), poll(NFAs)));
    }

    return NFAs.top();
}


NFA ComponentParser::addExpressionInBrackets(vector<component>& components, int *index) {
    std::vector<component> temp;
    int bracketsCount = 1;
    int j;
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
    if (bracketsCount != 0) {
        // bad parsing
        exit(-1);
    }
    *index = j;
    return buildParseTree(temp);
}

NFA ComponentParser::addClosure(component& comp, NFA&& nfa) {
    return comp.type == POS_CLOSURE ? Positive_closure(nfa) : Kleene_closure(nfa);
}

NFA ComponentParser::applyBinaryOperation(component_type type, NFA&& firstNFA, NFA&& secondNFA) {
    switch (type) {
        case TO:
        case CONCAT: return Concatenate(firstNFA, secondNFA);
        case OR: return Or(firstNFA, secondNFA);
        default: exit(-1);
    }
}

NFA ComponentParser::addCharNFA(RegularDefinition& regDefinition) {
    this->regToNFA[regDefinition] = NFA(regDefinition.name[0]);
}

NFA ComponentParser::applyToOperation(component& c1, component& c2) {
    char first = c1.regularDefinition.name[0];
    char second = c2.regularDefinition.name[1];
    if (second - first < 0) exit(-1);
    NFA nfa(first);
    while (++first >= second) {
        NFA next(first);
        nfa = Or(nfa, next);
    }
    return nfa;
}

