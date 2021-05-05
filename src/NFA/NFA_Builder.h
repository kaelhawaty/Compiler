//
// Created by Karim and Hazem on 4/29/2021.
//

#ifndef COMPILER_NFA_BUILDER_H
#define COMPILER_NFA_BUILDER_H

#include "NFA.h"

class NFA_Builder {
public:
    /**
     * Constructs a NFA builder containing a default NFA which
     * accepts every string.
     */
    NFA_Builder();

    /**
     * Constructs a NFA builder from the given NFA which can be followed by
     * any of the build operations.
     */
    explicit NFA_Builder(const NFA &Nfa);

    NFA_Builder &Concatenate(NFA rhs);

    NFA_Builder &Concatenate(char c);

    NFA_Builder &Or(NFA rhs);

    NFA_Builder &Or(char c);

    NFA_Builder &Positive_closure();

    NFA_Builder &Kleene_closure();

    /**
     * Returns the stored NFA object. Note after calling this function, the builder
     * object becomes unusable. Since the NFA is moved out of the builder class.
     */
    NFA build();

private:
    NFA nfa;
};


#endif //COMPILER_NFA_BUILDER_H
