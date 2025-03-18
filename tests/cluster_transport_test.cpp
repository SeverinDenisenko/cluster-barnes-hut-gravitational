#include <gtest/gtest.h>
#include <memory>

#include "cluster.hpp"
#include "transport.hpp"
#include "types.hpp"

namespace bh {

namespace {
    std::unique_ptr<node> g_node;
    int g_argc;
    char** g_argv;
}

class ClusterTestEnviroment : public testing::Environment {
public:
    explicit ClusterTestEnviroment(int argc, char** argv)
    {
        g_argc = argc;
        g_argv = argv;
    }
};

TEST(ClusterTransportTest, SetupTest)
{
    g_node = std::make_unique<node>(g_argc, g_argv);

    ASSERT_EQ(g_node->nodes_count(), 2);
}

TEST(ClusterTransportTest, SendReciveTest)
{
    cluster_transport transport;

    if (g_node->is_master()) {
        transport.send_struct<int>(42, g_node->slaves_node_indexes().front());
        transport.send_struct<float>(1.234f, g_node->slaves_node_indexes().front());
    } else {
        ASSERT_EQ(transport.receive_struct<int>(g_node->master_node_index()), 42);
        ASSERT_EQ(transport.receive_struct<float>(g_node->master_node_index()), 1.234f);
    }
}

TEST(ClusterTransportTest, ArraySendReciveTest)
{
    cluster_transport transport;

    array<int> int_array { 1, 2, 3, 4 };
    array<float> float_array { 1.0f, 2.0f, 3.0f, 4.0f };

    if (g_node->is_master()) {
        transport.send_array<int>(int_array, g_node->slaves_node_indexes().front());
        transport.send_array<float>(float_array, g_node->slaves_node_indexes().front());
    } else {
        ASSERT_EQ(transport.receive_array<int>(g_node->master_node_index()), int_array);
        ASSERT_EQ(transport.receive_array<float>(g_node->master_node_index()), float_array);
    }
}

TEST(ClusterTransportTest, ArrayIteratorSendReciveTest)
{
    cluster_transport transport;

    array<int> int_array { 1, 2, 3, 4 };
    array<float> float_array { 1.0f, 2.0f, 3.0f, 4.0f };

    if (g_node->is_master()) {
        transport.send_array<int>(int_array.begin(), int_array.end(), g_node->slaves_node_indexes().front());
        transport.send_array<float>(float_array.begin(), float_array.end(), g_node->slaves_node_indexes().front());
    } else {
        ASSERT_EQ(transport.receive_array<int>(g_node->master_node_index()), int_array);
        ASSERT_EQ(transport.receive_array<float>(g_node->master_node_index()), float_array);
    }
}

TEST(ClusterTransportTest, ArraySendIteratorReciveTest)
{
    cluster_transport transport;

    array<int> int_array { 1, 2, 3, 4 };
    array<float> float_array { 1.0f, 2.0f, 3.0f, 4.0f };

    if (g_node->is_master()) {
        transport.send_array<int>(int_array, g_node->slaves_node_indexes().front());
        transport.send_array<float>(float_array, g_node->slaves_node_indexes().front());
    } else {
        array<int> int_array_recv(4);
        array<float> float_array_recv(4);

        transport.receive_array<int>(int_array_recv.begin(), int_array_recv.end(), g_node->master_node_index());
        transport.receive_array<float>(float_array_recv.begin(), float_array_recv.end(), g_node->master_node_index());

        ASSERT_EQ(int_array_recv, int_array);
        ASSERT_EQ(float_array_recv, float_array);
    }
}

TEST(ClusterTransportTest, FinalizeTest)
{
    g_node = nullptr;
}

}

int main(int argc, char** argv)
{
    std::string command_line_arg(argc == 2 ? argv[1] : "");
    testing::InitGoogleTest(&argc, argv);
    testing::AddGlobalTestEnvironment(new bh::ClusterTestEnviroment(argc, argv));
    return RUN_ALL_TESTS();
}
