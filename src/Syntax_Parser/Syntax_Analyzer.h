//
// Created by hazem on 5/31/2021.
//

#ifndef COMPILER_SYNTAX_ANALYZER_H
#define COMPILER_SYNTAX_ANALYZER_H

#endif //COMPILER_SYNTAX_ANALYZER_H

struct Symbol {
    enum class Type {
        TERMINAL,
        NON_TERMINAL,
        EPSILON
    };
    std::string name;
    Type type;

    inline bool operator==(const Symbol &a) const {
        return name == a.name && type == a.type;
    }
};
namespace std
{
    template<>
    struct hash<Symbol>
    {
        size_t
        operator()(const Symbol &symbol) const noexcept {
            return hash<string>()(symbol.name);
        }
    };
}

// it's just assumption, can be changed later.
const Symbol eps_symbol = {"Є", Symbol::Type::EPSILON};

using Production = std::vector<Symbol>;
using Rule = std::vector<Production>;

// The sync production that appears in table entries to indicate synchronizing tokens.
const Production SYNC = {};
