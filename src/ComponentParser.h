//
// Created by Mahmoudmohamed on 4/29/2021.
//

#ifndef COMPILER_COMPONENTPARSER_H
#define COMPILER_COMPONENTPARSER_H


#include "NFAGenerator.h"
#include "NFA_Builder.h"

class ComponentParser {
private:
    unordered_map<string, NFA> regToNFA;
    NFA_Builder addExpressionInBrackets(std::vector<component>&, int* index);

    static NFA_Builder applyToOperation(component&, component&);
    static NFA_Builder addClosure(component&, NFA_Builder&&);
    static NFA_Builder applyBinaryOperation(component_type, NFA_Builder&&, NFA_Builder&&);

public:
    ComponentParser(unordered_map<string,NFA>& regularDefinitions): regToNfa(RegularDefinition) {}
    NFA buildParseTree(std::vector<component>&);
    NFA addCharNFA(char);
};


#endif //COMPILER_COMPONENTPARSER_H
