#include <cstdlib>
#include <gtest/gtest.h>
#include <vector>

#include "linalg.hpp"
#include "tree.hpp"
#include "types.hpp"

namespace bh {

struct point {
    vec2 position {};
    u32 amout {};
};

struct node {
    u32 sum {};
};

using test_quadtree = quadtree<point, node>;

TEST(QuadTreeTest, EmptyTest)
{
    std::vector<point> data = {};

    test_quadtree tree = test_quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 0);
}

TEST(QuadTreeTest, SingularTest)
{
    std::vector<point> data = { point { .position = vec2 { -1.0, -1.0 } } };

    test_quadtree tree = test_quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 1);
}

TEST(QuadTreeTest, ConstructTest)
{
    std::vector<point> data = { point { .position = vec2 { -1.0, -1.0 } },
                                point { .position = vec2 { -1.0, 1.0 } },
                                point { .position = vec2 { 1.0, -1.0 } },
                                point { .position = vec2 { 1.0, 1.0 } } };

    test_quadtree tree = test_quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 4 + 1);
}

TEST(QuadTreeTest, BalanceTest)
{
    std::vector<point> data = { point { .position = vec2 { -1.0, -1.0 } },
                                point { .position = vec2 { -1.0, 1.0 } },
                                point { .position = vec2 { 1.0, -1.0 } } };

    test_quadtree tree = test_quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 3 + 1);
}

TEST(QuadTreeTest, DuplicateTest)
{
    std::vector<point> data = { point { .position = vec2 { -1.0, -1.0 } },
                                point { .position = vec2 { -1.0, -1.0 } },
                                point { .position = vec2 { 1.0, 1.0 } },
                                point { .position = vec2 { 1.0, 1.0 } } };

    test_quadtree tree = test_quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 2 + 1);
}

TEST(QuadTreeTest, RebuildTest)
{
    std::vector<point> data = { point { .position = vec2 { -1.0, -1.0 } },
                                point { .position = vec2 { -1.0, 1.0 } },
                                point { .position = vec2 { 1.0, -1.0 } },
                                point { .position = vec2 { 1.0, 1.0 } } };

    test_quadtree tree = test_quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 4 + 1);

    test_quadtree::rebuild(tree);

    EXPECT_EQ(tree.node_count(), 4 + 1);
}

TEST(QuadTreeTest, ChangeRebuildTest)
{
    std::vector<point> data = { point { .position = vec2 { -1.0, -1.0 } },
                                point { .position = vec2 { -1.0, 1.0 } },
                                point { .position = vec2 { 1.0, -1.0 } },
                                point { .position = vec2 { 1.0, 1.0 } } };

    test_quadtree tree = test_quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 4 + 1);

    data[2] = data[0];
    data[3] = data[1];

    test_quadtree::rebuild(tree);

    EXPECT_EQ(tree.node_count(), 2 + 1);
}

TEST(QuadTreeTest, WalkLeafTest)
{
    std::vector<point> data       = { point { .position = vec2 { -1.0, -1.0 } },
                                      point { .position = vec2 { -1.0, 1.0 } },
                                      point { .position = vec2 { 1.0, -1.0 } },
                                      point { .position = vec2 { 1.0, 1.0 } } };
    std::vector<u8> external_data = { 1, 2, 3, 4 };

    test_quadtree tree = test_quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 4 + 1);

    u8 sum = 0;

    tree.walk_leafs([&sum, &external_data]([[maybe_unused]] u8 node, u8 point) { sum += external_data[point]; });

    EXPECT_EQ(sum, 10);
}

TEST(QuadTreeTest, WalkNodesTest)
{
    std::vector<point> data       = { point { .position = vec2 { -1.0, -1.0 } },
                                      point { .position = vec2 { -1.0, 1.0 } },
                                      point { .position = vec2 { 1.0, -1.0 } },
                                      point { .position = vec2 { 1.0, 1.0 } } };
    std::vector<u8> external_data = { 1, 1, 1, 1 };

    test_quadtree tree = test_quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 4 + 1);

    std::vector<u8> nodes_data(tree.node_count());

    tree.walk_leafs([&nodes_data, &external_data](u8 node, u8 point) { nodes_data[node] += external_data[point]; });

    tree.walk_nodes([&nodes_data](u8 node, u8 child) { nodes_data[node] += nodes_data[child]; });

    EXPECT_EQ(nodes_data[0], 4);
    EXPECT_EQ(nodes_data[1], 1);
    EXPECT_EQ(nodes_data[2], 1);
    EXPECT_EQ(nodes_data[3], 1);
    EXPECT_EQ(nodes_data[4], 1);
}

TEST(QuadTreeTest, WalkNodesTestBalance)
{
    std::vector<point> data = { point { .position = vec2 { -1.0, -1.0 } },
                                point { .position = vec2 { -1.0, -1.0 } },
                                point { .position = vec2 { 1.0, 1.0 } },
                                point { .position = vec2 { 1.0, 1.0 } } };

    std::vector<u8> external_data = { 1, 1, 1, 1 };

    test_quadtree tree = test_quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 2 + 1);

    std::vector<u8> nodes_data(tree.node_count());

    tree.walk_leafs([&nodes_data, &external_data](u8 node, u8 point) { nodes_data[node] += external_data[point]; });

    tree.walk_nodes([&nodes_data](u8 node, u8 child) { nodes_data[node] += nodes_data[child]; });

    EXPECT_EQ(nodes_data[0], 4);
    EXPECT_EQ(nodes_data[1], 2);
    EXPECT_EQ(nodes_data[2], 2);
}

TEST(QuadTreeTest, ReduceTest)
{
    std::vector<point> data       = { point { .position = vec2 { -1.0, -1.0 } },
                                      point { .position = vec2 { -1.0, 1.0 } },
                                      point { .position = vec2 { 1.0, -1.0 } },
                                      point { .position = vec2 { 1.0, 1.0 } } };
    std::vector<u8> external_data = { 1, 1, 1, 1 };

    test_quadtree tree = test_quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 4 + 1);

    std::vector<u8> nodes_data(tree.node_count());

    tree.walk_leafs([&nodes_data, &external_data](u8 node, u8 point) { nodes_data[node] += external_data[point]; });

    tree.walk_nodes([&nodes_data](u8 node, u8 child) { nodes_data[node] += nodes_data[child]; });

    u32 points_sum   = 0;
    u32 nodes_sum    = 0;
    u32 combined_sum = 0;

    tree.reduce(
        []([[maybe_unused]] u32 node) { return; },
        [&points_sum, &external_data](u32 point) { points_sum += external_data[point]; },
        []([[maybe_unused]] test_quadtree::axis_aligned_bounding_box aabb) -> bool { return false; });

    tree.reduce(
        [&nodes_sum, &nodes_data](u32 node) { nodes_sum += nodes_data[node]; },
        []([[maybe_unused]] u32 point) { return; },
        []([[maybe_unused]] test_quadtree::axis_aligned_bounding_box aabb) -> bool { return true; });

    tree.reduce(
        [&combined_sum, &nodes_data](u32 node) { combined_sum += nodes_data[node]; },
        [&combined_sum, &external_data](u32 point) { combined_sum += external_data[point]; },
        []([[maybe_unused]] test_quadtree::axis_aligned_bounding_box aabb) -> bool { return rand() % 2; });

    EXPECT_EQ(nodes_sum, 4);
    EXPECT_EQ(points_sum, 4);
    EXPECT_EQ(combined_sum, 4);
}

}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
