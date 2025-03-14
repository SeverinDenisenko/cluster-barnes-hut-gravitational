#include <gtest/gtest.h>

#include "linalg.hpp"

namespace bh {

TEST(VectorTest, MultiplyTest)
{
    vec2 vec_1 { 1.0, 1.0 };
    vec2 vec_2 { 0.1, 0.1 };

    EXPECT_EQ(vec_1 * 0.1, vec_2);
    EXPECT_EQ(0.1 * vec_1, vec_2);
}

TEST(VectorTest, DivideTest)
{
    vec2 vec_1 { 1.0, 1.0 };
    vec2 vec_2 { 0.1, 0.1 };

    EXPECT_EQ(vec_1 / 10.0, vec_2);
}

TEST(VectorTest, ChainTest)
{
    vec2 vec_1 { 1.0, 1.0 };
    vec2 vec_2 { 0.005, 0.005 };

    vec2 vec_3 = vec_1 * 0.1 * 0.1 / 2.0;

    EXPECT_TRUE((vec_3 - vec_2).len() < 1e-10);
}

TEST(VectorTest, PlusMinusTest)
{
    vec2 vec_1 { 1.0, -1.0 };
    vec2 vec_2 { -0.5, 0.5 };

    EXPECT_TRUE((vec_1 - vec_2 - vec2 { 1.5, -1.5 }).len() < 1e-10);
    EXPECT_TRUE((vec_1 + vec_2 - vec2 { 0.5, -0.5 }).len() < 1e-10);
}

}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
