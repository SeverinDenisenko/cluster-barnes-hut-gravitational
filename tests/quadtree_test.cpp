#include <gtest/gtest.h>
#include <vector>

#include "linalg.hpp"
#include "tree.hpp"

namespace bh {

TEST(QuadTreeTest, ConstructTest)
{
    std::vector<vec2> data = { vec2 { -1.0, 1.0 }, vec2 { 2.0, 1.0 }, vec2 { 3.0, 1.0 }, vec2 { 2.0, 4.0 } };

    quadtree tree = quadtree::build(data);
}

}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
