//
// Created by Karim and Hazem on 4/29/2021.
//

#ifndef COMPILER_NFA_BUILDER_H
#define COMPILER_NFA_BUILDER_H

#include "NFA.h"

class NFA_Builder {
public:
    NFA_Builder();

    explicit NFA_Builder(const NFA &Nfa);

    NFA_Builder &Concatenate(NFA rhs);

    NFA_Builder &Or(NFA rhs);

    NFA_Builder &Positive_closure();

    NFA_Builder &Kleene_closure();

    NFA build();

private:
    NFA nfa;
};


#endif //COMPILER_NFA_BUILDER_H
