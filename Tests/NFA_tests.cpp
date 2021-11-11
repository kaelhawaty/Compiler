#include "gtest/gtest.h"
#include <climits>
#include "../src/NFA/NFA.h"
#include "../src/NFA/NFA_Builder.h"

namespace NFA_test {

    /**
     * Note that this function tests the equality of two NFA by their deterministic
     * behavior and not by strict equality of NFA graphs as this is what we require
     * from two different NFA.
     *
     * In case of a need for strict equality, we would need to assign every node in
     * a transition to the corresponding node in the other graph by trying out all of
     * the permutation. For our case, two NFAs are equal if they behave the same.
     */
    bool dfs(NFA::Set a, NFA::Set b, std::unordered_map<NFA::Set, NFA::Set> &visited) {
        const int is_visited_a = visited.count(a);
        const int is_visited_b = visited.count(b);
        if(is_visited_a != is_visited_b){
            return false;
        }
        if (is_visited_a) {
            return visited[a] == b;
        }
        visited[a] = b;
        visited[b] = a;
        bool ans = true;
        for (char c = 0; c < CHAR_MAX && ans; c++) {
            ans = ans && dfs(Move(a, c), Move(b, c), visited);
        }
        return ans;
    }

    bool areEqual(const NFA &a, const NFA &b) {
        std::unordered_map<NFA::Set, NFA::Set> visited;
        return dfs(NFA::Set{a.get_start()}, NFA::Set{b.get_start()}, visited);
    }

    TEST(NFAAreEqualTest, Identity) {
        NFA nfa('a');
        EXPECT_TRUE(areEqual(nfa, nfa));
    }

    TEST(NFAAreEqualTest, DifferentTransition) {
        NFA a('a');
        NFA b('b');
        EXPECT_FALSE(areEqual(a, b));
    }

    TEST(NFA_test, SingleChar) {
        NFA nfa('a');
        std::vector<std::unique_ptr<NFA::Node>> nodes;
        nodes.emplace_back(std::make_unique<NFA::Node>());
        auto start = nodes.back().get();
        nodes.emplace_back(std::make_unique<NFA::Node>());
        auto end = nodes.back().get();
        start->addTransition('a', end);

        NFA temp{start, end, std::move(nodes)};
        EXPECT_TRUE(areEqual(nfa, temp));
    }

    TEST(NFA_test, NFACopy) {
        std::vector<std::unique_ptr<NFA::Node>> nodes;
        for (int i = 0; i < 1000; i++) {
            nodes.emplace_back(std::make_unique<NFA::Node>());
        }
        char c = 'a';
        for (int i = 0; i < nodes.size(); i++) {
            for (int j = 0; j < nodes.size(); j++) {
                nodes[i]->addTransition(c, nodes[j].get());
            }
        }

        auto start = nodes[0].get(), end = nodes.back().get();
        NFA temp{start, end, std::move(nodes)};
        NFA cpy{temp};
        EXPECT_TRUE(areEqual(temp, cpy));
    }

    TEST(NFA_test, Concatenate) {
        NFA res = NFA_Builder(NFA{'a'}).Concatenate('b').build();
        // res = * a * e * b *
        std::vector<std::unique_ptr<NFA::Node>> nodes(4);
        for (auto &arrNode : nodes) {
            arrNode = std::make_unique<NFA::Node>();
        }

        std::vector<char> transitions{'a', EPSILON, 'b'};
        for (int i = 0; i < nodes.size() - 1; i++) {
            nodes[i]->addTransition(transitions[i], nodes[i + 1].get());
        }
        auto start = nodes[0].get(), end = nodes.back().get();
        NFA temp{start, end, std::move(nodes)};
        EXPECT_TRUE(areEqual(temp, res));
    }

    TEST(NFA_test, Or) {
        NFA res = NFA_Builder(NFA{'a'}).Or('b').build();
        /*    e * a * e
         *  *           *
         *    e * b * e
         */
        std::vector<std::unique_ptr<NFA::Node>> nodes(6);
        for (auto &arrNode : nodes) {
            arrNode = std::make_unique<NFA::Node>();
        }
        /*    e 1 a 2 e
         *  0           5
         *    e 3 b 4 e
         */
        nodes[0]->addTransition(EPSILON, nodes[1].get());
        nodes[0]->addTransition(EPSILON, nodes[3].get());
        nodes[1]->addTransition('a', nodes[2].get());
        nodes[2]->addTransition(EPSILON, nodes[5].get());
        nodes[3]->addTransition('b', nodes[4].get());
        nodes[4]->addTransition(EPSILON, nodes[5].get());

        auto start = nodes[0].get(), end = nodes.back().get();
        NFA temp(start, end, std::move(nodes));
        EXPECT_TRUE(areEqual(temp, res));
    }

    TEST(NFA_test, Positive_closure) {
        NFA res = NFA_Builder(NFA{'a'}).Positive_closure().build();
        /*        e
         *      /   \
         *  * e * a * e *
         */
        std::vector<std::unique_ptr<NFA::Node>> nodes(4);
        for (auto &arrNode : nodes) {
            arrNode = std::make_unique<NFA::Node>();
        }
        /*        e
         *      /   \
         *  0 e 1 a 2 e 3
         */
        nodes[0]->addTransition(EPSILON, nodes[1].get());
        nodes[1]->addTransition('a', nodes[2].get());
        nodes[2]->addTransition(EPSILON, nodes[3].get());
        nodes[2]->addTransition(EPSILON, nodes[1].get());

        auto start = nodes[0].get(), end = nodes.back().get();
        NFA temp{start, end, std::move(nodes)};
        EXPECT_TRUE(areEqual(temp, res));
    }

    TEST(NFA_test, Kleene_closure) {
        NFA res = NFA_Builder(NFA{'a'}).Kleene_closure().build();
        /*        e
         *    /   e    \
         *   /  /   \   \
         *  * e * a * e *
         */
        std::vector<std::unique_ptr<NFA::Node>> nodes(4);
        for (auto &arrNode : nodes) {
            arrNode = std::make_unique<NFA::Node>();
        }
        /*        e
         *    /   e   \
         *   /  /   \  \
         *  0 e 1 a 2 e 3
         */
        nodes[0]->addTransition(EPSILON, nodes[1].get());
        nodes[1]->addTransition('a', nodes[2].get());
        nodes[2]->addTransition(EPSILON, nodes[3].get());
        nodes[2]->addTransition(EPSILON, nodes[1].get());
        // Only difference between Positive_closure and Kleene_closure.
        nodes[0]->addTransition(EPSILON, nodes[3].get());

        auto start = nodes[0].get(), end = nodes.back().get();
        NFA temp{start, end, std::move(nodes)};
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
        NFA Or = NFA_Builder().Concatenate('a').Or('b').build();
        NFA closureA = NFA_Builder().Concatenate('a').Kleene_closure().build();
        NFA res = NFA_Builder().Concatenate('a').Concatenate('a').Concatenate(Or).Concatenate(closureA).build();
        EXPECT_TRUE(MatchRegexp("aabaaaaa", res));
        EXPECT_TRUE(MatchRegexp("aaaaaaaa", res));
        EXPECT_TRUE(MatchRegexp("aaa", res));
        EXPECT_FALSE(MatchRegexp("baaaaaaa", res));
        EXPECT_FALSE(MatchRegexp("aaab", res));
        EXPECT_FALSE(MatchRegexp("aacaaa", res));
    }


}
