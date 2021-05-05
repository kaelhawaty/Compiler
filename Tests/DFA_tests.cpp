//
// Created by hazem on 5/3/2021.
//
#include "gtest/gtest.h"
#include "../src/NFA/NFA_Builder.h"
#include "../src/Parser/RegularExpression.h"
#include "../src/DFA/DFA.h"

namespace DFA_tests {
    const char DEFAULT_CHAR = 'a';

    TEST(DFAConstruction, SingleNFANoMinimize) {
        // a
        // NFA: 0 -a-> 1
        // DFA: 0 -a-> <2>
        //      |
        //      1
        // start state is 0, phi state is 1 and final(accepting) state is 2
        RegularExpression exp(std::string{DEFAULT_CHAR}, 1, NFA{DEFAULT_CHAR});
        DFA dfa({exp});
        const std::vector<DFA::State> &states = dfa.getStates();
        const int start_state = 0, phi_state = 1, end_state = 2;
        EXPECT_TRUE(states.size() == 3);
        EXPECT_TRUE(!states[start_state].isAcceptingState &&
                    !states[phi_state].isAcceptingState &&
                    states[end_state].isAcceptingState);// check that state 2 is accepting state and state 1, 0 are not.
        EXPECT_TRUE(states[end_state].regEXP == exp.getName());
        //  All states point to the phi state under all inputs with the exception of the starting state pointing
        //  to the end state under input DEFAULT_CHAR.
        for (char c = 1; c < CHAR_MAX; ++c) {
            if (c != DEFAULT_CHAR) {
                EXPECT_TRUE(states[start_state].transitions.at(c) == phi_state);
            } else {
                EXPECT_TRUE(states[start_state].transitions.at(c) == end_state);
            }
            EXPECT_TRUE(states[phi_state].transitions.at(c) == phi_state);
            EXPECT_TRUE(states[end_state].transitions.at(c) == phi_state);
        }
    }

    TEST(DFAConstruction, MultiNFAWithDiffPrioritesNoMinimize) {
        NFA aa = NFA_Builder().Concatenate(DEFAULT_CHAR).Concatenate(DEFAULT_CHAR).build();
        NFA a_kclosure = NFA_Builder().Concatenate(DEFAULT_CHAR).Kleene_closure().build();
        DFA dfa({{"aa", 1, aa},
                 {"a*", 2, a_kclosure}});
        // NFA:  1 -a-> 2 -L-> 3 -a-> 4
        //               <--L--
        //              |      |
        //       5 -L-> 6 -a-> 7 -L-> 8
        //       |                    |
        //        ---------L---------->
        //
        //         a*          a*           aa         a*
        // DFA:  <1568> -a-> <23678> -a-> <4678> -a-> <678>
        //          0           2            3          4
        // phi_state is state number 1.
        // all states go to phi_state for any input other than the above.
        // state 3 is ending state for 'aa' and 'a*' but its regular expression is aa
        // because it has lower priority i.e earlier in the input file.
        const std::vector<DFA::State> &states = dfa.getStates();
        constexpr int num_states = 5;
        EXPECT_TRUE(states.size() == num_states);
        std::array<bool, num_states> expected_flag{true, false, true, true, true};
        std::array<std::string, num_states> expected_name{"a*", "", "a*", "aa", "a*"};
        for (int i = 0; i < num_states; ++i) {
            EXPECT_EQ(states[i].isAcceptingState, expected_flag[i]) << "Failed at state #" << i;
            EXPECT_EQ(states[i].regEXP, expected_name[i]) << "Failed at state #" << i;
        }
        const int phi_state = 1;
        EXPECT_TRUE(states[0].transitions.at(DEFAULT_CHAR) == 2);
        EXPECT_TRUE(states[1].transitions.at(DEFAULT_CHAR) == 1);
        EXPECT_TRUE(states[2].transitions.at(DEFAULT_CHAR) == 3);
        EXPECT_TRUE(states[3].transitions.at(DEFAULT_CHAR) == 4);
        EXPECT_TRUE(states[4].transitions.at(DEFAULT_CHAR) == 4);
        for (const auto &state : states) {
            for (char c = 1; c < CHAR_MAX; ++c) {
                if (c != DEFAULT_CHAR) {
                    EXPECT_TRUE(state.transitions.at(c) == phi_state);
                }
            }
        }
    }

    bool dfs(int nodeA, int nodeB, const std::vector<DFA::State> &a, const std::vector<DFA::State> &b,
             std::unordered_set<const DFA::State *> &visited) {
        const DFA::State *a_ptr = &a[nodeA];
        const DFA::State *b_ptr = &b[nodeB];
        if (visited.count(a_ptr) ^ visited.count(b_ptr) || a_ptr->isAcceptingState != b_ptr->isAcceptingState ||
            a_ptr->regEXP != b_ptr->regEXP) {
            return false;
        }
        if (visited.count(a_ptr)) {
            return true;
        }
        visited.insert(a_ptr);
        visited.insert(b_ptr);
        bool ans = true;
        for (char c = 0; c < CHAR_MAX && ans; c++) {
            ans = ans && dfs(a[nodeA].transitions[c], b[nodeB].transitions[c], a, b, visited);
        }
        return ans;
    }

    bool areEqual(const std::vector<DFA::State> &a, const std::vector<DFA::State> &b) {
        std::unordered_set<const DFA::State *> visited;
        return dfs(0, 0, a, b, visited);
    }

    void set_state_arguments(DFA::State &state, const int id, const bool is_accepting, const std::string &regExp) {
        state.id = id;
        state.isAcceptingState = is_accepting;
        state.regEXP = regExp;
    }

    TEST(DFAAreEqualTest, Identity) {
        DFA dfa({{"", 1, NFA{DEFAULT_CHAR}}});
        EXPECT_TRUE(areEqual(dfa.getStates(), dfa.getStates()));
    }

    TEST(DFAAreEqualTest, DifferentProperties) {
        // DFA:    0 -a> 1 -b> 2 -a
        //               |        | -> 4 (Accepting_state "default")
        //               |--c> 3 -c
        //
        // With each state pointing to state phi (5) for any other input.
        constexpr int num_expected_nodes = 6;
        constexpr int phi_state = 5;
        DFA::State DEFAULT_STATE = DFA::State{0};
        std::fill(DEFAULT_STATE.transitions.begin(), DEFAULT_STATE.transitions.end(), phi_state);
        std::vector<DFA::State> expected(num_expected_nodes, DEFAULT_STATE);

        set_state_arguments(expected[0], 0, false, "");
        expected[0].transitions['a'] = 1;

        set_state_arguments(expected[1], 1, false, "");
        expected[1].transitions['b'] = 2;
        expected[0].transitions['c'] = 3;

        set_state_arguments(expected[2], 2, false, "");
        expected[2].transitions['a'] = 4;

        set_state_arguments(expected[3], 3, false, "");
        expected[3].transitions['c'] = 4;

        set_state_arguments(expected[4], 4, true, "default");

        // Copy the state diagram and adding a transition from 3 to 4 with a different char.
        std::vector<DFA::State> actual = expected;

        EXPECT_TRUE(areEqual(actual, expected));

        int old_val = actual[3].transitions['b'];
        actual[3].transitions['b'] = 4;

        EXPECT_FALSE(areEqual(actual, expected));

        // Reset previous change and check for different acceptance state.
        actual[3].transitions['b'] = old_val;
        actual[2].isAcceptingState = true;

        EXPECT_FALSE(areEqual(actual, expected));

        // Reset previous change and check for different regular expression.
        actual[2].isAcceptingState = false;
        actual[4].regEXP = "Non-default";

        EXPECT_FALSE(areEqual(actual, expected));

        // Reset previous change and check that they are equal after reverting changes.
        actual[4].regEXP = "default";

        EXPECT_TRUE(areEqual(actual, expected));
    }

    TEST(DFAConstruction, SimpleMinimization) {
        // Regular expression: {aa}*
        NFA aa_kclosure = NFA_Builder().Concatenate(DEFAULT_CHAR).Concatenate(DEFAULT_CHAR).Kleene_closure().build();
        DFA dfa({{"{aa}*", 1, aa_kclosure}});
        // The expected DFA pre-minimization:
        // Accepting:   T     F     T
        //              0 -a> 1 -a> 2
        //                    |     |
        //                    |<-a--|
        // With each state pointing to state phi (3) for any input other than DEFAULT_CHAR.
        //
        // The minimized DFA:
        // Accepting:     T     F
        //                0 -a> 1
        //                |     |
        //                |<-a--|
        // With each state pointing to state phi (2) for any input other than DEFAULT_CHAR.
        constexpr int num_expected_nodes = 3;
        constexpr int phi_state = 2;
        DFA::State DEFAULT_STATE = DFA::State{0};
        std::fill(DEFAULT_STATE.transitions.begin(), DEFAULT_STATE.transitions.end(), phi_state);
        std::vector<DFA::State> expected(num_expected_nodes, DEFAULT_STATE);

        set_state_arguments(expected[0], 0, true, "{aa}*");
        expected[0].transitions[DEFAULT_CHAR] = 1;

        set_state_arguments(expected[1], 1, false, "");
        expected[1].transitions[DEFAULT_CHAR] = 0;

        EXPECT_TRUE(areEqual(dfa.getStates(), expected));
    }

    TEST(DFAConstruction, IdentityMinimization) {
        // Regular expression: {aa}*
        NFA aa_kclosure = NFA_Builder().Concatenate(DEFAULT_CHAR).Concatenate(DEFAULT_CHAR).Kleene_closure().build();
        DFA dfa({{"{aa}*", 1, aa_kclosure},
                 {"{aa}*", 2, aa_kclosure}});
        // Since both regular expressions are identical, the minimized DFA shouldn't change.
        // The minimized DFA:
        // Accepting:     T     F
        //                0 -a> 1
        //                |     |
        //                |<-a--|
        // With each state pointing to state phi (2) for any input other than DEFAULT_CHAR.
        constexpr int num_expected_nodes = 3;
        constexpr int phi_state = 2;
        DFA::State DEFAULT_STATE = DFA::State{0};
        std::fill(DEFAULT_STATE.transitions.begin(), DEFAULT_STATE.transitions.end(), phi_state);
        std::vector<DFA::State> expected(num_expected_nodes, DEFAULT_STATE);

        set_state_arguments(expected[0], 0, true, "{aa}*");
        expected[0].transitions[DEFAULT_CHAR] = 1;

        set_state_arguments(expected[1], 1, false, "");
        expected[1].transitions[DEFAULT_CHAR] = 0;

        EXPECT_TRUE(areEqual(dfa.getStates(), expected));
    }

    TEST(DFAConstruction, ComplexNFA) {
        // Regular expression: c | cb (a | b)*
        NFA lhs = NFA_Builder().Concatenate('c').build();
        NFA closure = NFA_Builder().Concatenate('a').Or('b').Kleene_closure().build();
        NFA rhs = NFA_Builder().Concatenate('c').Concatenate('b').Concatenate(std::move(closure)).build();
        DFA dfa({{"c",           1, lhs},
                 {"cb (a | b)*", 2, rhs}});
        // The expected DFA:
        // Accepting:         c    cb (a | b)*
        //              0 -c> 1 -b> 2 ------
        //                          |       |
        //                          |<-a|b--|
        // With each state pointing to state phi (3) for any other input.
        constexpr int num_expected_nodes = 4;
        constexpr int phi_state = 3;
        DFA::State DEFAULT_STATE = DFA::State{0};
        std::fill(DEFAULT_STATE.transitions.begin(), DEFAULT_STATE.transitions.end(), phi_state);
        std::vector<DFA::State> expected(num_expected_nodes, DEFAULT_STATE);

        set_state_arguments(expected[0], 0, false, "");
        expected[0].transitions['c'] = 1;

        set_state_arguments(expected[1], 1, true, "c");
        expected[1].transitions['b'] = 2;

        set_state_arguments(expected[2], 2, true, "cb (a | b)*");
        expected[2].transitions['a'] = 2;
        expected[2].transitions['b'] = 2;

        EXPECT_TRUE(areEqual(dfa.getStates(), expected));
    }

    TEST(DFAConstruction, ComplexNFA2) {
        // Regular expression: (d* (c |a))*
        NFA res = NFA_Builder().Concatenate('d').Kleene_closure().Concatenate(
                NFA_Builder(NFA{'c'}).Or('a').build()).Kleene_closure().build();
        DFA dfa({{"(d* (c |a))*", 1, res}});
        // The expected DFA:
        // Accepting:(d* (c |a))*
        //              <---c|a----
        //              |         |
        //      ------> 0  --d->  1 <----
        //      |       |         |     |
        //      |<-c|a--|         |<-d--|
        // With each state pointing to state phi (3) for any other input.
        constexpr int num_expected_nodes = 3;
        constexpr int phi_state = 2;
        DFA::State DEFAULT_STATE = DFA::State{0};
        std::fill(DEFAULT_STATE.transitions.begin(), DEFAULT_STATE.transitions.end(), phi_state);
        std::vector<DFA::State> expected(num_expected_nodes, DEFAULT_STATE);

        set_state_arguments(expected[0], 0, true, "(d* (c |a))*");
        expected[0].transitions['a'] = 0;
        expected[0].transitions['c'] = 0;
        expected[0].transitions['d'] = 1;

        set_state_arguments(expected[1], 1, false, "");
        expected[1].transitions['a'] = 0;
        expected[1].transitions['c'] = 0;
        expected[1].transitions['d'] = 1;

        EXPECT_TRUE(areEqual(dfa.getStates(), expected));
    }
}
