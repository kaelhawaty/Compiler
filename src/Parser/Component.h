//
// Created by Mahmoudmohamed on 4/30/2021.
//

#ifndef COMPILER_COMPONENT_H
#define COMPILER_COMPONENT_H

#include <utility>

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
    component(component_type type, std::string regDef) : type(type), regularDefinition(std::move(regDef)) {}

    explicit component(component_type type) : type(type), regularDefinition("") {}

    friend bool operator==(const component &lhs, const component &rhs) {
        return lhs.type == rhs.type && lhs.regularDefinition == rhs.regularDefinition;
    }

    component_type type;
    std::string regularDefinition;
};

#endif //COMPILER_COMPONENT_H
