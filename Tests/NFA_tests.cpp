#include "gtest/gtest.h"
#include "../src/NFA/NFA.h"
#include "../src/NFA/NFA_Builder.h"

namespace NFA_test {

    bool dfs(NFA::Set a, NFA::Set b, std::unordered_set<const NFA::Node *> &visitedA,
             std::unordered_set<const NFA::Node *> &visitedB) {
        if (a.size() != b.size()) {
            return false;
        }
        if (a.size() == 0) {
            return true;
        }
        for (auto it = a.begin(); it != a.end();) {
            if (visitedA.count(*it)) {
                it = a.erase(it);
            } else {
                visitedA.insert(*it);
                it++;
            }
        }
        for (auto it = b.begin(); it != b.end();) {
            if (visitedB.count(*it)) {
                it = b.erase(it);
            } else {
                visitedB.insert(*it);
                it++;
            }
        }
        if (a.size() != b.size()) {
            return false;
        }
        bool ans = true;
        for (char c = 0; c < CHAR_MAX && ans; c++) {
            ans = ans && dfs(Move(a, c), Move(b, c), visitedA, visitedB);
        }
        return ans;
    }

    bool areEqual(const NFA &a, const NFA &b) {
        NFA::Set stA{a.get_start()};
        NFA::Set stB{b.get_start()};
        std::unordered_set<const NFA::Node *> visitedA;
        std::unordered_set<const NFA::Node *> visitedB;
        return dfs(stA, stB, visitedA, visitedB);
    }

    TEST(AreEqualTest, Identity) {
        NFA nfa('a');
        EXPECT_TRUE(areEqual(nfa, nfa));
    }

    TEST(AreEqualTest, DifferentTransitionEquality) {
        NFA a('a');
        NFA b('b');
        EXPECT_FALSE(areEqual(a, b));
    }

    TEST(NFA_test, SingleChar) {
        NFA nfa('a');
        auto nodeA = std::make_shared<NFA::Node>();
        auto nodeB = std::make_shared<NFA::Node>();
        nodeA->addTransition('a', nodeB);
        NFA temp{nodeA, nodeB};
        EXPECT_TRUE(areEqual(nfa, temp));
    }

    TEST(NFA_test, NFACopy) {
        std::array<std::shared_ptr<NFA::Node>, 1000> arr;
        for (auto &i : arr) {
            i = std::make_shared<NFA::Node>();
        }
        char c = 'a';
        for (int i = 0; i < arr.size(); i++) {
            for (int j = 0; j < arr.size(); j++) {
                arr[i]->addTransition(c, arr[j]);
            }
        }

        NFA temp{arr[0], arr.back()};
        NFA cpy{temp};
        EXPECT_TRUE(areEqual(temp, cpy));
    }

    TEST(NFA_test, Concatenate) {
        NFA a{'a'};
        NFA b{'b'};

        NFA_Builder builder{a};
        builder.Concatenate(b);
        NFA res = builder.build();
        // res = * a * e * b *
        std::vector<std::shared_ptr<NFA::Node>> arrNodes(4);
        for (auto &arrNode : arrNodes) {
            arrNode = std::make_shared<NFA::Node>();
        }

        std::vector<char> transitions{'a', EPSILON, 'b'};
        for (int i = 0; i < arrNodes.size() - 1; i++) {
            arrNodes[i]->addTransition(transitions[i], arrNodes[i + 1]);
        }
        NFA temp{arrNodes[0], arrNodes.back()};
        EXPECT_TRUE(areEqual(temp, res));
    }

    TEST(NFA_test, Or) {
        NFA a{'a'};
        NFA b{'b'};
        NFA_Builder builder{a};
        builder.Or(b);
        NFA res = builder.build();
        /*    e * a * e
         *  *           *
         *    e * b * e
         */
        std::vector<std::shared_ptr<NFA::Node>> arrNodes(6);
        for (auto &arrNode : arrNodes) {
            arrNode = std::make_shared<NFA::Node>();
        }
        /*    e 1 a 2 e
         *  0           5
         *    e 3 b 4 e
         */
        arrNodes[0]->addTransition(EPSILON, arrNodes[1]);
        arrNodes[0]->addTransition(EPSILON, arrNodes[3]);
        arrNodes[1]->addTransition('a', arrNodes[2]);
        arrNodes[2]->addTransition(EPSILON, arrNodes[5]);
        arrNodes[3]->addTransition('b', arrNodes[4]);
        arrNodes[4]->addTransition(EPSILON, arrNodes[5]);
        NFA temp(arrNodes[0], arrNodes.back());
        EXPECT_TRUE(areEqual(temp, res));
    }

    TEST(NFA_test, Positive_closure) {
        NFA a{'a'};
        NFA_Builder builder{a};
        builder.Positive_closure();
        NFA res = builder.build();
        /*        e
         *      /   \
         *  * e * a * e *
         */
        std::vector<std::shared_ptr<NFA::Node>> arrNodes(4);
        for (auto &arrNode : arrNodes) {
            arrNode = std::make_shared<NFA::Node>();
        }
        /*        e
         *      /   \
         *  0 e 1 a 2 e 3
         */
        arrNodes[0]->addTransition(EPSILON, arrNodes[1]);
        arrNodes[1]->addTransition('a', arrNodes[2]);
        arrNodes[2]->addTransition(EPSILON, arrNodes[3]);
        arrNodes[2]->addTransition(EPSILON, arrNodes[1]);
        NFA temp{arrNodes[0], arrNodes.back()};
        EXPECT_TRUE(areEqual(temp, res));
    }

    TEST(NFA_test, Kleene_closure) {
        NFA a{'a'};
        NFA_Builder builder{a};
        builder.Kleene_closure();
        NFA res = builder.build();
        /*        e
         *    /   e    \
         *   /  /   \   \
         *  * e * a * e *
         */
        std::vector<std::shared_ptr<NFA::Node>> arrNodes(4);
        for (auto &arrNode : arrNodes) {
            arrNode = std::make_shared<NFA::Node>();
        }
        /*        e
         *    /   e   \
         *   /  /   \  \
         *  0 e 1 a 2 e 3
         */
        arrNodes[0]->addTransition(EPSILON, arrNodes[1]);
        arrNodes[1]->addTransition('a', arrNodes[2]);
        arrNodes[2]->addTransition(EPSILON, arrNodes[3]);
        arrNodes[2]->addTransition(EPSILON, arrNodes[1]);
        // Only difference between Positive_closure and Kleene_closure.
        arrNodes[0]->addTransition(EPSILON, arrNodes[3]);
        NFA temp{arrNodes[0], arrNodes.back()};
        EXPECT_TRUE(areEqual(temp, res));
    }

    bool MatchRegexp(const std::string &s, const NFA &nfa) {
        NFA::Set st{nfa.get_start()};
        st = E_closure(st);
        for (char c: s) {
            st = E_closure(Move(st, c));
        }
        if (st.count(nfa.get_end())) {
            return true;
        }
        return false;
    }

    TEST(NFA_test, MatchesRegExp) {
        // aa (a | b) a*
        NFA_Builder builder{NFA('a')};
        builder.Concatenate(NFA('a'));
        NFA_Builder OrBuilder{NFA('a')};
        OrBuilder.Or(NFA('b'));
        builder.Concatenate(OrBuilder.build());
        NFA_Builder closureA(NFA('a'));
        closureA.Kleene_closure();
        builder.Concatenate(closureA.build());

        NFA res = builder.build();
        EXPECT_TRUE(MatchRegexp("aabaaaaa", res));
        EXPECT_TRUE(MatchRegexp("aaaaaaaa", res));
        EXPECT_TRUE(MatchRegexp("aaa", res));
        EXPECT_FALSE(MatchRegexp("baaaaaaa", res));
        EXPECT_FALSE(MatchRegexp("aaab", res));
        EXPECT_FALSE(MatchRegexp("aacaaa", res));
    }


}
