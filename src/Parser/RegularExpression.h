//
// Created by Mahmoudmohamed on 4/30/2021.
//

#ifndef COMPILER_REGULAREXPRESSION_H
#define COMPILER_REGULAREXPRESSION_H

#include <utility>

#include "string"
#include "../NFA/NFA.h"

class RegularExpression {
private:
    std::string name;
    int priority;
    NFA nfa;

public:
    RegularExpression(const std::string &name, int priority, const NFA &nfa) : name(name),
                                                                         priority(priority),
                                                                         nfa(nfa) {}

    // for testing purposes.
    const NFA &getNFA() const {
        return nfa;
    }

    const std::string &getName() const {
        return name;
    }

    const int getPriority() const {
        return priority;
    }
};

#endif //COMPILER_REGULAREXPRESSION_H
