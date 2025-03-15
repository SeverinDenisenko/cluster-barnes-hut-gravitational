#include <gtest/gtest.h>

#include "linalg.hpp"

namespace bh {

TEST(VectorTest, MultiplyTest)
{
    vec2 vec_1 { 1.0f, 1.0f };
    vec2 vec_2 { 0.1f, 0.1f };

    EXPECT_EQ(vec_1 * 0.1f, vec_2);
    EXPECT_EQ(0.1f * vec_1, vec_2);
}

TEST(VectorTest, DivideTest)
{
    vec2 vec_1 { 1.0f, 1.0f };
    vec2 vec_2 { 0.1f, 0.1f };

    EXPECT_EQ(vec_1 / 10.0f, vec_2);
}

TEST(VectorTest, ChainTest)
{
    vec2 vec_1 { 1.0f, 1.0f };
    vec2 vec_2 { 0.005f, 0.005f };

    vec2 vec_3 = vec_1 * 0.1f * 0.1f / 2.0f;

    EXPECT_TRUE((vec_3 - vec_2).len() < 1e-10);
}

TEST(VectorTest, PlusMinusTest)
{
    vec2 vec_1 { 1.0f, -1.0f };
    vec2 vec_2 { -0.5f, 0.5f };

    EXPECT_TRUE((vec_1 - vec_2 - vec2 { 1.5f, -1.5f }).len() < 1e-10f);
    EXPECT_TRUE((vec_1 + vec_2 - vec2 { 0.5f, -0.5f }).len() < 1e-10f);
}

}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
