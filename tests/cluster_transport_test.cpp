#include <chrono>
#include <gtest/gtest.h>
#include <memory>
#include <thread>

#include "cluster.hpp"
#include "ev_loop.hpp"
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

    if (g_node->node_index() == 0) {
        transport.send_struct<int>(42, 1, 100);
        transport.send_struct<float>(1.234f, 1, 101);
    } else {
        ASSERT_EQ(transport.receive_struct<int>(0, 100), 42);
        ASSERT_EQ(transport.receive_struct<float>(0, 101), 1.234f);
    }
}

TEST(ClusterTransportTest, ArraySendReciveTest)
{
    cluster_transport transport;

    array<int> int_array { 1, 2, 3, 4 };
    array<float> float_array { 1.0f, 2.0f, 3.0f, 4.0f };

    if (g_node->node_index() == 0) {
        transport.send_array<int>(int_array, 1, 102);
        transport.send_array<float>(float_array, 1, 103);
    } else {
        ASSERT_EQ(transport.receive_array<int>(0, 102), int_array);
        ASSERT_EQ(transport.receive_array<float>(0, 103), float_array);
    }
}

TEST(ClusterTransportTest, ArrayIteratorSendReciveTest)
{
    cluster_transport transport;

    array<int> int_array { 1, 2, 3, 4 };
    array<float> float_array { 1.0f, 2.0f, 3.0f, 4.0f };

    if (g_node->node_index() == 0) {
        transport.send_array<int>(int_array.begin(), int_array.end(), 1, 104);
        transport.send_array<float>(float_array.begin(), float_array.end(), 1, 105);
    } else {
        ASSERT_EQ(transport.receive_array<int>(0, 104), int_array);
        ASSERT_EQ(transport.receive_array<float>(0, 105), float_array);
    }
}

TEST(ClusterTransportTest, ArraySendIteratorReciveTest)
{
    cluster_transport transport;

    array<int> int_array { 1, 2, 3, 4 };
    array<float> float_array { 1.0f, 2.0f, 3.0f, 4.0f };

    if (g_node->node_index() == 0) {
        transport.send_array<int>(int_array, 1, 107);
        transport.send_array<float>(float_array, 1, 108);
    } else {
        array<int> int_array_recv { 0, 0, 0, 0 };
        array<float> float_array_recv { 0.0, 0.0, 0.0, 0.0 };

        transport.receive_array<int>(int_array_recv.begin(), int_array_recv.end(), 0, 107);
        transport.receive_array<float>(float_array_recv.begin(), float_array_recv.end(), 0, 108);

        ASSERT_EQ(int_array_recv, int_array);
        ASSERT_EQ(float_array_recv, float_array);
    }
}

TEST(ClusterTransportTest, SendAsyncReciveTest)
{
    using namespace std::chrono_literals;

    if (g_node->node_index() == 0) {
        pushToEvLoop<unit>([](unit) -> unit {
            cluster_transport transport;

            transport.send_struct<u32>(42, 1, 106);

            return unit();
        });

        pushToEvLoop<unit>([](unit) -> unit {
            cluster_transport transport;

            transport.add_handler<u32>(1, 107, [](u32 number) -> unit {
                EXPECT_EQ(42, number);

                return unit();
            });

            return unit();
        });

    } else {
        pushToEvLoop<unit>([](unit) -> unit {
            cluster_transport transport;

            transport.add_handler<u32>(0, 106, [](u32 number) -> unit {
                EXPECT_EQ(40, number);

                return unit();
            });

            return unit();
        });

        pushToEvLoop<unit>([](unit) -> unit {
            cluster_transport transport;

            transport.send_struct<u32>(42, 0, 107);

            return unit();
        });
    }
}

TEST(ClusterTransportTest, FinalizeTest)
{
    pushToEvLoop<unit>([](unit) -> unit {
        g_node = nullptr;

        stopEvLoop();

        return unit();
    });
}

}

int main(int argc, char** argv)
{
    std::string command_line_arg(argc == 2 ? argv[1] : "");
    testing::InitGoogleTest(&argc, argv);
    testing::AddGlobalTestEnvironment(new bh::ClusterTestEnviroment(argc, argv));

    bh::startEvLoop([](bh::unit) -> bh::unit {
        (void)RUN_ALL_TESTS();

        return bh::unit();
    });

    return 0;
}
