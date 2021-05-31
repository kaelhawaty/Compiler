//
// Created by Mahmoudmohamed on 5/30/2021.
//

#ifndef COMPILER_SYNTAX_UTILIZATION_H
#define COMPILER_SYNTAX_UTILIZATION_H

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "CFG_Reader.h"

using namespace std;

class Syntax_Utilization {

private:
    unordered_map<string, unordered_set<string>> first;
    unordered_map<string, unordered_set<string>> follow;

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
    void first_calculate(const string&, const unordered_map<string, vector<vector<Symbol>>> &, unordered_set<string> &);

    /**
     *  Applies the second rule of calculating the follow sets Using the given grammar rules.
     */
    void follow_calculate_by_first(const unordered_map<string, vector<vector<Symbol>>> &);

    /**
     *  Applies the third rule of calculating the follow sets Using the given grammar rules.
     */
    void follow_calculate_by_follow(const unordered_map<string, vector<vector<Symbol>>> &);

    bool find(unordered_map<string, unordered_set<string>>& ,const string &, unordered_set<string> &);

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
    void follow_calculate(const unordered_map<string, vector<vector<Symbol>>> &, const string &);

public:
    /**
     * Constructing the Follow and First set using unordered_map of rules,
     * and the Start symbol.
     */
    Syntax_Utilization(const unordered_map<string, vector<vector<Symbol>>> &, const string &);

    /**
     *  Takes 2 parameters
     *  First parameter is the name of the non-terminal symbol.
     *  Second one is the the unordered_set where the first of the non-terminal will be stored there.
     *  @return true if there's a non-terminal symbol with that name, False otherwise.
     */
    bool first_of(const string &, unordered_set<string> &);

    /**
     *  Takes 2 parameters
     *  First parameter is the name of the non-terminal symbol.
     *  Second one is the the unordered_set where the follow of the non-terminal will be stored there.
     *  @return true if there's a non-terminal symbol with that name, False otherwise.
     */
    bool follow_of(const string &, unordered_set<string> &);

};


#endif //COMPILER_SYNTAX_UTILIZATION_H
