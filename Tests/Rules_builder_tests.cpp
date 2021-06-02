//
// Created by hazem on 5/30/2021.
//
#include <fstream>
#include "gtest/gtest.h"
#include "../src/Syntax_Parser/Rules_builder.h"
#include "Syntax_tests_helper.h"

namespace CFG_Reader_tests {
    class Rules_builder_tests : public ::testing::Test {
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

    TEST_F(Rules_builder_tests, simpleTest) {
        writeRules("# A = 'b' '+' 'c'");
        Rules_builder reader(tempCFGRulesFilePath);
        ASSERT_TRUE(reader.getRules().size() == 1);
        ASSERT_TRUE(reader.getStartSymbol().name == "A");
        ASSERT_TRUE(reader.getRules().at(reader.getStartSymbol()).size() == 1);
        ASSERT_TRUE(reader.getRules().at(reader.getStartSymbol())[0].size() == 3);
        auto productions = reader.getRules().at(reader.getStartSymbol())[0];
        Production expected = writeSymbols({"'b'", "'+'", "'c'"});
        ASSERT_TRUE(productions == expected);
    }

    TEST_F(Rules_builder_tests, testEpsilon){
        writeRules("# A = B | C | \\L");
        Rules_builder reader(tempCFGRulesFilePath);
        ASSERT_TRUE(reader.getRules().size() == 1);
        ASSERT_TRUE(reader.getStartSymbol().name == "A");
        ASSERT_TRUE(reader.getRules().at(reader.getStartSymbol()).size() == 3);
        ASSERT_TRUE(reader.getRules().at(reader.getStartSymbol())[2].front().type == Symbol::Type::EPSILON);
    }

    TEST_F(Rules_builder_tests, testSimpleLeftRecursion) {
        writeRules("# A = A a | B");
        // A --> A a | B
        // --------------
        // A --> B A'
        // A' --> a A' | E
        Rules_builder reader(tempCFGRulesFilePath);
        reader.buildLL1Grammar();
        ASSERT_TRUE(reader.getRules().size() == 2);
        ASSERT_TRUE(reader.getStartSymbol().name == "A");
        Production expected_A = writeSymbols({"B", "A'"});
        Production expected_A_dash = writeSymbols({"a", "A'"});
        Symbol A_dash = {reader.getStartSymbol().name+"\'", Symbol::Type::NON_TERMINAL};
        ASSERT_TRUE(reader.getRules().at(reader.getStartSymbol()).size() == 1 && reader.getRules().at(A_dash).size() == 2);
        ASSERT_TRUE(reader.getRules().at(reader.getStartSymbol())[0] == expected_A);
        ASSERT_TRUE(reader.getRules().at(A_dash)[0] == expected_A_dash);
        ASSERT_TRUE(reader.getRules().at(A_dash)[1][0] == eps_symbol);

    }
}
