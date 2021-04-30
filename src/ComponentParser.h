//
// Created by Mahmoudmohamed on 4/29/2021.
//

#ifndef COMPILER_COMPONENTPARSER_H
#define COMPILER_COMPONENTPARSER_H


#include "RegularDefinition.h"
#include "NFAGenerator.h"


class ComponentParser {
    unordered_map<string, NFA> regToNFA;
public:
    ComponentParser(){}
    NFA buildParseTree(std::vector<component>&);
    NFA addExpressionInBrackets(std::vector<component>&, int* index);
    NFA addCharNFA(RegularDefinition&);

    static NFA applyToOperation(component&, component&);
    static NFA addClosure(component&, NFA&&);
    static NFA applyBinaryOperation(component_type, NFA&&, NFA&&);

};


#endif //COMPILER_COMPONENTPARSER_H
