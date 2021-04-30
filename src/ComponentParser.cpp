//
// Created by Mahmoudmohamed on 4/29/2021.
//

#include <stack>
#include "ComponentParser.h"
#include "NFA_Builder.h"

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

    std::stack<NFA_Builder> NFABuilders;
    std::stack<component_type> operations;

    for (int i = 0; i < components.size(); i++) {
        component comp = components[i];
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
        else if (comp.type == OPEN_BRACKETS) {
            NFABuilders.push(addExpressionInBrackets(components, &i));
        }
        else if (comp.type == REG_EXP || comp.type == RED_DEF) {
            NFABuilders.push(NFA_Builder(regToNFA[comp.regularDefinition]));
        }
        else if (comp.type == CONCAT || comp.type == OR) {
            while (!operations.empty() && hasPrecedence(operations.top(), comp.type)) {
                NFABuilders.push(applyBinaryOperation(poll(operations), poll(NFABuilders), poll(NFABuilders)));
            }
            operations.push(comp.type);
        }
    }

    while (!operations.empty()) {
        NFABuilders.push(applyBinaryOperation(poll(operations), poll(NFABuilders), poll(NFABuilders)));
    }

    return NFABuilders.top().build();
}


NFA_Builder ComponentParser::addExpressionInBrackets(vector<component>& components, int *index) {
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
    return NFA_Builder(buildParseTree(temp));
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

NFA ComponentParser::addCharNFA(char regDefinitionChar) {
    return NFA(regDefinitionChar);
}

NFA_Builder ComponentParser::applyToOperation(component& c1, component& c2) {
    char first = c1.regularDefinition[0];
    char second = c2.regularDefinition[0];
    if (second - first < 0) exit(-1);
    NFA_Builder nfa_builder((NFA(first)));
    while (++first >= second) {
        nfa_builder.Or(NFA (first));
    }
    return nfa_builder;
}

