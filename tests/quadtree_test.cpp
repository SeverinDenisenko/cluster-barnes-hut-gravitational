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
    std::vector<point> data = { point { .position = vec2 { -1.0f, -1.0f } } };

    test_quadtree tree = test_quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 1);
}

TEST(QuadTreeTest, ConstructTest)
{
    std::vector<point> data = { point { .position = vec2 { -1.0f, -1.0f } },
                                point { .position = vec2 { -1.0f, 1.0f } },
                                point { .position = vec2 { 1.0f, -1.0f } },
                                point { .position = vec2 { 1.0f, 1.0f } } };

    test_quadtree tree = test_quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 4 + 1);
}

TEST(QuadTreeTest, BalanceTest)
{
    std::vector<point> data = { point { .position = vec2 { -1.0f, -1.0f } },
                                point { .position = vec2 { -1.0f, 1.0f } },
                                point { .position = vec2 { 1.0f, -1.0f } } };

    test_quadtree tree = test_quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 3 + 1);
}

TEST(QuadTreeTest, DuplicateTest)
{
    std::vector<point> data = { point { .position = vec2 { -1.0f, -1.0f } },
                                point { .position = vec2 { -1.0f, -1.0f } },
                                point { .position = vec2 { 1.0f, 1.0f } },
                                point { .position = vec2 { 1.0f, 1.0f } } };

    test_quadtree tree = test_quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 2 + 1);
}

TEST(QuadTreeTest, RebuildTest)
{
    std::vector<point> data = { point { .position = vec2 { -1.0f, -1.0f } },
                                point { .position = vec2 { -1.0f, 1.0f } },
                                point { .position = vec2 { 1.0f, -1.0f } },
                                point { .position = vec2 { 1.0f, 1.0f } } };

    test_quadtree tree = test_quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 4 + 1);

    test_quadtree::rebuild(tree);

    EXPECT_EQ(tree.node_count(), 4 + 1);
}

TEST(QuadTreeTest, ChangeRebuildTest)
{
    std::vector<point> data = { point { .position = vec2 { -1.0f, -1.0f } },
                                point { .position = vec2 { -1.0f, 1.0f } },
                                point { .position = vec2 { 1.0f, -1.0f } },
                                point { .position = vec2 { 1.0f, 1.0f } } };

    test_quadtree tree = test_quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 4 + 1);

    data[2] = data[0];
    data[3] = data[1];

    test_quadtree::rebuild(tree);

    EXPECT_EQ(tree.node_count(), 2 + 1);
}

TEST(QuadTreeTest, WalkLeafTest)
{
    std::vector<point> data = { point { .position = vec2 { -1.0f, -1.0f }, .amout = 1 },
                                point { .position = vec2 { -1.0f, 1.0f }, .amout = 2 },
                                point { .position = vec2 { 1.0f, -1.0f }, .amout = 3 },
                                point { .position = vec2 { 1.0f, 1.0f }, .amout = 4 } };

    test_quadtree tree = test_quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 4 + 1);

    u8 sum = 0;

    tree.walk_leafs([&sum]([[maybe_unused]] node& n, point& p) { sum += p.amout; });

    EXPECT_EQ(sum, 10);
}

TEST(QuadTreeTest, WalkNodesTest)
{
    std::vector<point> data = { point { .position = vec2 { -1.0f, -1.0f }, .amout = 1 },
                                point { .position = vec2 { -1.0f, 1.0f }, .amout = 1 },
                                point { .position = vec2 { 1.0f, -1.0f }, .amout = 1 },
                                point { .position = vec2 { 1.0f, 1.0f }, .amout = 1 } };

    test_quadtree tree = test_quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 4 + 1);

    tree.walk_leafs([](node& n, point& p) { n.sum += p.amout; });

    tree.walk_nodes([](node& n, node& c) { n.sum += c.sum; });

    EXPECT_EQ(tree.get_node(0).sum, 4);
    EXPECT_EQ(tree.get_node(1).sum, 1);
    EXPECT_EQ(tree.get_node(2).sum, 1);
    EXPECT_EQ(tree.get_node(3).sum, 1);
    EXPECT_EQ(tree.get_node(4).sum, 1);
}

TEST(QuadTreeTest, WalkNodesTestBalance)
{
    std::vector<point> data = { point { .position = vec2 { -1.0f, -1.0f }, .amout = 1 },
                                point { .position = vec2 { -1.0f, -1.0f }, .amout = 1 },
                                point { .position = vec2 { 1.0f, 1.0f }, .amout = 1 },
                                point { .position = vec2 { 1.0f, 1.0f }, .amout = 1 } };

    test_quadtree tree = test_quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 2 + 1);

    tree.walk_leafs([](node& n, point& p) { n.sum += p.amout; });

    tree.walk_nodes([](node& n, node& c) { n.sum += c.sum; });

    EXPECT_EQ(tree.get_node(0).sum, 4);
    EXPECT_EQ(tree.get_node(1).sum, 2);
    EXPECT_EQ(tree.get_node(2).sum, 2);
}

TEST(QuadTreeTest, ReduceTest)
{
    std::vector<point> data = { point { .position = vec2 { -1.0f, -1.0f }, .amout = 1 },
                                point { .position = vec2 { -1.0f, 1.0f }, .amout = 1 },
                                point { .position = vec2 { 1.0f, -1.0f }, .amout = 1 },
                                point { .position = vec2 { 1.0f, 1.0f }, .amout = 1 } };

    test_quadtree tree = test_quadtree::build(data);

    EXPECT_EQ(tree.node_count(), 4 + 1);

    tree.walk_leafs([](node& n, point& p) { n.sum += p.amout; });

    tree.walk_nodes([](node& n, node& c) { n.sum += c.sum; });

    u32 points_sum   = 0;
    u32 nodes_sum    = 0;
    u32 combined_sum = 0;

    tree.reduce(
        []([[maybe_unused]] const node& node) { return; },
        [&points_sum](const point& point) { points_sum += point.amout; },
        []([[maybe_unused]] const test_quadtree::axis_aligned_bounding_box aabb) -> bool { return false; });

    tree.reduce(
        [&nodes_sum](const node& node) { nodes_sum += node.sum; },
        []([[maybe_unused]] const point& point) { return; },
        []([[maybe_unused]] const test_quadtree::axis_aligned_bounding_box aabb) -> bool { return true; });

    tree.reduce(
        [&combined_sum](const node& node) { combined_sum += node.sum; },
        [&combined_sum](const point& point) { combined_sum += point.amout; },
        []([[maybe_unused]] const test_quadtree::axis_aligned_bounding_box aabb) -> bool { return rand() % 2; });

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
