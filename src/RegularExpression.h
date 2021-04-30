//
// Created by Mahmoudmohamed on 4/30/2021.
//

#ifndef COMPILER_REGULAREXPRESSION_H
#define COMPILER_REGULAREXPRESSION_H

#include <utility>

#include "string"
#include "NFA.h"

class RegularExpression {
private:
    std::string expression;
    std::string name;
    int priority;
    NFA nfa;

public:
    RegularExpression(std::string& exp, std::string& name, int priority, NFA& nfa) : expression(exp),
                                                                                     name(name),
                                                                                     priority(priority),
                                                                                     nfa(nfa){}
};

#endif //COMPILER_REGULAREXPRESSION_H
