//
// Created by Mahmoudmohamed on 4/29/2021.
//

#ifndef COMPILER_COMPONENTPARSER_H
#define COMPILER_COMPONENTPARSER_H
#include <string>
#include "Component.h"
#include "../NFA/NFA_Builder.h"

class ComponentParser {
private:
    // Maps regular definitions to its NFA.
    std::unordered_map<std::string, NFA> regToNFA;
    NFA_Builder addExpressionInBrackets(const std::vector<component>&, int* index);

    static NFA CharToNFA(const component&);
    static NFA_Builder applyToOperation(const component&, const component&);
    static NFA_Builder addClosure(const component&, NFA_Builder);
    static NFA_Builder applyBinaryOperation(component_type, NFA_Builder, NFA_Builder);

public:
    /**
     * Takes vector of regular definitions and its corresponding components, and return Map between
     * regular definitions and their corresponding NFAs
     */
    const std::unordered_map<std::string, NFA>& regDefinitionsToNFAs(const std::vector<std::pair<std::string, std::vector<component>>>&);

    /**
     * Take vector of component of a Regular definition and return single NFA.
     */
    NFA SingleRegDefToNFA(const std::vector<component>&);

};


#endif //COMPILER_COMPONENTPARSER_H
