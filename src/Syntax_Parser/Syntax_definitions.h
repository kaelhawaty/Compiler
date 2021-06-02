#include <utility>

//
// Created by hazem on 5/31/2021.
//

#ifndef COMPILER_SYNTAX_ANALYZER_H
#define COMPILER_SYNTAX_ANALYZER_H

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
const Symbol eps_symbol = {"#", Symbol::Type::EPSILON};

using Production = std::vector<Symbol>;

class Rule : public std::vector<Production> {
public:
    explicit Rule(std::string lhsName) {
        lhs = {std::move(lhsName), Symbol::Type::NON_TERMINAL};
    }

    explicit Rule(std::string lhsName, std::vector<Production> vec) : std::vector<Production>(std::move(vec)) {
        lhs = {std::move(lhsName), Symbol::Type::NON_TERMINAL};
    }

    const Symbol &get_lhs() const {
        return this->lhs;
    }

private:
    Symbol lhs;
};

// The sync production that appears in table entries to indicate synchronizing tokens.
const Production SYNC_PRODUCTION = {};

#endif //COMPILER_SYNTAX_ANALYZER_H
