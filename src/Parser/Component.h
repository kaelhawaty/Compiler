//
// Created by Mahmoudmohamed on 4/30/2021.
//

#ifndef COMPILER_COMPONENT_H
#define COMPILER_COMPONENT_H

#include "string"

enum component_type {
    REG_EXP,
    RED_DEF,
    EQUAL,
    CLOSE_BRACKETS,
    OPEN_BRACKETS,
    POS_CLOSURE,
    KLEENE_CLOSURE,
    CONCAT,
    OR,
    TO
};
struct component {
    component_type type;
    std::string regularDefinition;
};

#endif //COMPILER_COMPONENT_H
