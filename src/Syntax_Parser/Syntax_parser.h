//
// Created by Karim on 6/2/2021.
//

#ifndef COMPILER_SYNTAX_PARSER_H
#define COMPILER_SYNTAX_PARSER_H

#include <string>
#include <vector>
#include <memory>
#include "Syntax_definitions.h"
#include "ParsingTable.h"
#include "../Parser/LexicalParser.h"

class Syntax_parser {
public:
    Syntax_parser(const std::unordered_map<Symbol, Rule>& rules, const Symbol &staring_symbol);

    enum class Status {
        ACCEPTED,
        ACCEPTED_WITH_ERRORS,
        NOT_MATCHED
    };

    std::pair<std::vector<std::vector<Symbol>>, Status> parse(LexicalParser &tokenizer) const;

    bool fail() const;

private:
    bool has_error{};
    Symbol starting_symbol;
    std::unique_ptr<ParsingTable> table;

    enum class Behavior {
        MATCH_TERMINAL, ENTRY_EXISTS, SYNC_ENTRY, NO_ENTRY
    };

    Syntax_parser::Behavior get_behavior(const Symbol &cur_sym, const Symbol &token_sym) const;
};


#endif //COMPILER_SYNTAX_PARSER_H
