#include <gtest/gtest.h>
#include <vector>

#include "linalg.hpp"
#include "tree.hpp"

namespace bh {

TEST(QuadTreeTest, EmptyTest)
{
    std::vector<vec2> data = {};

    quadtree tree = quadtree::build(std::move(data));

    EXPECT_EQ(tree.node_count(), 0);
}

TEST(QuadTreeTest, SingularTest)
{
    std::vector<vec2> data = { vec2 { -1.0, -1.0 } };

    quadtree tree = quadtree::build(std::move(data));

    EXPECT_EQ(tree.node_count(), 1);
}

TEST(QuadTreeTest, ConstructTest)
{
    std::vector<vec2> data = { vec2 { -1.0, -1.0 }, vec2 { -1.0, 1.0 }, vec2 { 1.0, -1.0 }, vec2 { 1.0, 1.0 } };

    quadtree tree = quadtree::build(std::move(data));

    EXPECT_EQ(tree.node_count(), 4 + 1);
}

}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
