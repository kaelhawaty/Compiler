//
// Created by hazem on 5/30/2021.
//
#include <fstream>
#include "gtest/gtest.h"
#include "../../src/Syntax_Parser/CFG_Reader.h"

namespace CFG_Reader_tests {
    class CFG_Reader_tests : public ::testing::Test {
    protected:
        std::fstream tempCFGRulesFile;
        std::string tempCFGRulesFilePath{::testing::TempDir() + "tempRules.txt"};

        void SetUp() override {
            tempCFGRulesFile.open(tempCFGRulesFilePath, std::fstream::in | std::fstream::out | std::fstream::trunc);
            ASSERT_TRUE(tempCFGRulesFile.is_open());
        }

        template<typename... Args>
        void writeRules(Args &&... lines) {
            ((tempCFGRulesFile << lines << "\n"), ...);
            tempCFGRulesFile.flush();
        }

        void TearDown() override {
            if (tempCFGRulesFile.is_open()) {
                tempCFGRulesFile.close();
            }
            std::remove(tempCFGRulesFilePath.c_str());
        }
    };

    TEST_F(CFG_Reader_tests, simpleTest){
        writeRules("# A = B '+' C");
        CFG_Reader reader(tempCFGRulesFilePath);
        ASSERT_TRUE(reader.rules.size() == 1);
        ASSERT_TRUE(reader.start_symbol == "A");
        ASSERT_TRUE(reader.rules[reader.start_symbol].size() == 1);
        ASSERT_TRUE(reader.rules[reader.start_symbol][0].size() == 3);
        auto productions = reader.rules[reader.start_symbol][0];
        CFG_Reader::Production expected = {{"B", CFG_Reader::Type::NON_TERMINAL},
                                           {"+", CFG_Reader::Type::TERMINAL},
                                           {"C", CFG_Reader::Type::NON_TERMINAL}};
        ASSERT_TRUE(productions == expected);
    }

    TEST_F(CFG_Reader_tests, testEpsilon){
        writeRules("# A = B | C | \\L");
        CFG_Reader reader(tempCFGRulesFilePath);
        ASSERT_TRUE(reader.rules.size() == 1);
        ASSERT_TRUE(reader.start_symbol == "A");
        ASSERT_TRUE(reader.rules[reader.start_symbol].size() == 3);
        ASSERT_TRUE(reader.rules[reader.start_symbol][2].front().type == CFG_Reader::Type::EPSILON);
    }

    TEST_F(CFG_Reader_tests, testSimpleLeftRecursion) {
        writeRules("# A = A a | B");
        // A --> A a | B
        // --------------
        // A --> B A'
        // A' --> a A' | E
        CFG_Reader reader(tempCFGRulesFilePath);
        ASSERT_TRUE(reader.rules.size() == 2);
        ASSERT_TRUE(reader.start_symbol == "A");
        CFG_Reader::Production expected_A = {{"B", CFG_Reader::Type::NON_TERMINAL},
                                           {"A\'", CFG_Reader::Type::NON_TERMINAL}};
        CFG_Reader::Production expected_A_dash = {{"a", CFG_Reader::Type::NON_TERMINAL},
                                           {"A\'", CFG_Reader::Type::NON_TERMINAL}};
        CFG_Reader::Production Epsilon = {{"\\L", CFG_Reader::Type::EPSILON}};
        ASSERT_TRUE(reader.rules[reader.start_symbol].size() == 1 && reader.rules[reader.start_symbol+"\'"].size() == 2);
        ASSERT_TRUE(reader.rules[reader.start_symbol][0] == expected_A);
        ASSERT_TRUE(reader.rules[reader.start_symbol+"\'"][0] == expected_A_dash);
        ASSERT_TRUE(reader.rules[reader.start_symbol+"\'"][1] == Epsilon);

    }
}
