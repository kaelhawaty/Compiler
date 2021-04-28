#include "gtest/gtest.h"

int factorial(int x){
    return x ? x * factorial(x-1) : 1;
}
TEST(FactorialTest, HandlesPositiveInput){
    EXPECT_EQ(factorial(3), 6);
}
