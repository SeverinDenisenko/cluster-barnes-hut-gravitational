#include <gtest/gtest.h>
#include <vector>

#include "linalg.hpp"
#include "tree.hpp"
#include "types.hpp"

namespace bh {

TEST(QuadTreeTest, EmptyTest)
{
    std::vector<vec2> data = {};

    quadtree tree = quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 0);
}

TEST(QuadTreeTest, SingularTest)
{
    std::vector<vec2> data = { vec2 { -1.0, -1.0 } };

    quadtree tree = quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 1);
}

TEST(QuadTreeTest, ConstructTest)
{
    std::vector<vec2> data = { vec2 { -1.0, -1.0 }, vec2 { -1.0, 1.0 }, vec2 { 1.0, -1.0 }, vec2 { 1.0, 1.0 } };

    quadtree tree = quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 4 + 1);
}

TEST(QuadTreeTest, BalanceTest)
{
    std::vector<vec2> data = { vec2 { -1.0, -1.0 }, vec2 { -1.0, 1.0 }, vec2 { 1.0, -1.0 } };

    quadtree tree = quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 3 + 1);
}

TEST(QuadTreeTest, DuplicateTest)
{
    std::vector<vec2> data = { vec2 { -1.0, -1.0 }, vec2 { -1.0, -1.0 }, vec2 { 1.0, 1.0 }, vec2 { 1.0, 1.0 } };

    quadtree tree = quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 2 + 1);
}

TEST(QuadTreeTest, RebuildTest)
{
    std::vector<vec2> data = { vec2 { -1.0, -1.0 }, vec2 { -1.0, 1.0 }, vec2 { 1.0, -1.0 }, vec2 { 1.0, 1.0 } };

    quadtree tree = quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 4 + 1);

    quadtree::rebuild(tree);

    EXPECT_EQ(tree.node_count(), 4 + 1);
}

TEST(QuadTreeTest, ChangeRebuildTest)
{
    std::vector<vec2> data = { vec2 { -1.0, -1.0 }, vec2 { -1.0, 1.0 }, vec2 { 1.0, -1.0 }, vec2 { 1.0, 1.0 } };

    quadtree tree = quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 4 + 1);

    data[2] = data[0];
    data[3] = data[1];

    quadtree::rebuild(tree);

    EXPECT_EQ(tree.node_count(), 2 + 1);
}

TEST(QuadTreeTest, WalkLeafTest)
{
    std::vector<vec2> data        = { vec2 { -1.0, -1.0 }, vec2 { -1.0, 1.0 }, vec2 { 1.0, -1.0 }, vec2 { 1.0, 1.0 } };
    std::vector<u8> external_data = { 1, 2, 3, 4 };

    quadtree tree = quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 4 + 1);

    u8 sum = 0;

    tree.walk([&sum, &external_data]([[maybe_unused]] u8 node, u8 point) { sum += external_data[point]; });

    EXPECT_EQ(sum, 10);
}

TEST(QuadTreeTest, WalkNodesTest)
{
    std::vector<vec2> data        = { vec2 { -1.0, -1.0 }, vec2 { -1.0, 1.0 }, vec2 { 1.0, -1.0 }, vec2 { 1.0, 1.0 } };
    std::vector<u8> external_data = { 1, 1, 1, 1 };

    quadtree tree = quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 4 + 1);

    std::vector<u8> nodes_data(tree.node_count());

    tree.walk([&nodes_data, &external_data](u8 node, u8 point) { nodes_data[node] += external_data[point]; });

    tree.walk([&nodes_data](u8 node, u8 child_a, u8 child_b, u8 child_c, u8 child_d) {
        nodes_data[node] = nodes_data[child_a] + nodes_data[child_b] + nodes_data[child_c] + nodes_data[child_d];
    });

    EXPECT_EQ(nodes_data[0], 4);
    EXPECT_EQ(nodes_data[1], 1);
    EXPECT_EQ(nodes_data[2], 1);
    EXPECT_EQ(nodes_data[3], 1);
    EXPECT_EQ(nodes_data[4], 1);
}

TEST(QuadTreeTest, WalkNodesTestBalance)
{
    std::vector<vec2> data        = { vec2 { -1.0, -1.0 }, vec2 { -1.0, -1.0 }, vec2 { 1.0, 1.0 }, vec2 { 1.0, 1.0 } };
    std::vector<u8> external_data = { 1, 1, 1, 1 };

    quadtree tree = quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 2 + 1);

    std::vector<u8> nodes_data(tree.node_count());

    tree.walk([&nodes_data, &external_data](u8 node, u8 point) { nodes_data[node] += external_data[point]; });

    tree.walk([&nodes_data](u8 node, u8 child_a, u8 child_b, u8 child_c, u8 child_d) {
        nodes_data[node] = nodes_data[child_a] + nodes_data[child_b] + nodes_data[child_c] + nodes_data[child_d];
    });

    EXPECT_EQ(nodes_data[0], 4);
    EXPECT_EQ(nodes_data[1], 2);
    EXPECT_EQ(nodes_data[2], 2);
}

}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
