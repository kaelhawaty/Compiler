//
// Created by Mahmoudmohamed on 5/30/2021.
//

#ifndef COMPILER_SYNTAX_UTILS_H
#define COMPILER_SYNTAX_UTILS_H

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "Syntax_Analyzer.h"


class Syntax_Utils {
public:

    using First_set = std::unordered_set<Symbol>;
    using Follow_set = std::unordered_set<Symbol>;
    using Terminal_set = std::unordered_set<Symbol>;

    /**
     * Constructing the Follow and First set using unordered_map of rules,
     * and the Start symbol.
     */
    Syntax_Utils(const std::unordered_map<Symbol, Rule> &, const Symbol &);

    /**
     *  @return if the symbol is non-terminal, it will return the FIRST(symbol),
     *  otherwise it will return the symbol name.
     */
    First_set first_of(const Symbol &);

    First_set first_of(const Production&);

    /**
     *  The symbol must be non-terminal symbol.
     *  @return if the symbol is non-terminal, it will return the FOLLOW(symbol).
     */
    Follow_set follow_of(const Symbol &);

private:
    std::unordered_map<Symbol, First_set> first;
    std::unordered_map<Symbol, Follow_set> follow;

    /**
     *To compute FIRST(X) for all grammar symbols X, it applies the following rules
     *   until no more terminals or E: can be added to any FIRST set.
     *   1. If X is a terminal, then FIRST(X) = {X}.
     *   2. If X is a non-terminal and X -> Y1 Y2 .... Yk is a production for some k >= 1,
     *      then place a in FIRST(X) if for some i, a is in FIRST(Yi), and Є is in all of
     *      FIRST(Y1), . . . , FIRST(Yi-1); that is, Y1 ... Yi-1 --> Є. If Є is in FIRST(Yj)
     *      for all j = 1,2, . . . , k, then add Є to FIRST(X). For example, everything
     *      in FIRST(Y1) is surely in FIRST(X). If Y1 does not derive Є, then we add
     *      nothing more to FIRST(X), but if Y1 -> Є, then we add FIRST(Y2), and
     *      So on.
     *  3. If X -> Є is a production, then add Є to FIRST(X).
     */
    void precompute_first(const Symbol &, const std::unordered_map<Symbol, Rule> &);

    /**
     *  To compute FOLLOW(A) for all non-terminals A, apply the following rules
     *  until nothing can be added to any FOLLOW set.
     *      1. Place $ in FOLLOW(S), where S is the start symbol, and $ is the input
     *         right endmarker.
     *      2. If there is a production A -> aBP, then everything in FIRST(P) except Є
     *         is in FOLLOW(B).
     *      3. If there is a production A -> aB, or a production A -> aBP, where
     *         FIRST(P) contains Є, then everything in FOLLOW (A) is in FOLLOW (B)
     */
    void precompute_follow(const std::unordered_map<Symbol, Rule> &, const Symbol &);

    /**
     *  Applies the second rule of calculating the follow sets Using the given grammar rules.
     */
    void follow_calculate_by_first(const std::unordered_map<Symbol, Rule> &);

    /**
     *  Applies the third rule of calculating the follow sets Using the given grammar rules.
     */
    void follow_calculate_by_follow(const std::unordered_map<Symbol, Rule> &);

    static Terminal_set find(std::unordered_map<Symbol, Terminal_set>& ,const Symbol &);

    /**
     * it insert all terminal Symbols from the src to dest,
     * @return true if any symbol get actually inserted in the dest, false otherwise.
     */
    static bool insert_into(Syntax_Utils::Terminal_set &dest, const Syntax_Utils::Terminal_set &src);

};

#endif //COMPILER_SYNTAX_UTILS_H
