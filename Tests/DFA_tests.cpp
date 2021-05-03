//
// Created by hazem on 5/3/2021.
//
#include "gtest/gtest.h"
#include "../src/NFA/NFA_Builder.h"
#include "../src/Parser/RegularExpression.h"
#include "../src/DFA/DFA.h"

namespace DFA_tests {
    const char DEFAULT_CHAR = 'a';

    TEST(DFAConstruction, SingleNFA) {
        NFA nfa(DEFAULT_CHAR);
        // a
        // NFA: 0 -a-> 1
        // DFA: 0 -a-> <2>
        //      |
        //      1
        // start state is 0, phi state is 1 and final(accepting) state is 2
        RegularExpression exp(std::string{DEFAULT_CHAR}, 1, nfa);
        DFA dfa({exp});
        const std::vector<DFA::State> &states = dfa.getStates();
        const int start_state = 0, phi_state = 1, end_state = 2;
        EXPECT_TRUE(states.size() == 3);// check that number of states is 3.
        EXPECT_TRUE(!states[start_state].isAcceptingState &&
                    !states[phi_state].isAcceptingState &&
                    states[end_state].isAcceptingState);// check that state 2 is accepting state and state 1, 0 are not.
        EXPECT_TRUE(states[end_state].regEXP == exp.getName());
        // check that start state go to state 2 under input 'a' only and go to phi state for other inputs.
        // check that state 1, 2 go to state 1 under all inputs.
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

    TEST(DFAConstruction, MultiNFAWithDiffPriorites) {

        NFA a(DEFAULT_CHAR);
        NFA aa = NFA_Builder(a).Concatenate(a).build();// aa priority 1
        NFA a_kclosure = NFA_Builder(a).Kleene_closure().build();// a* priority 2
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
        // state 3 is ending state for 'aa' and 'a*' but its regular expression is aa because it has higher priority.
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
}
