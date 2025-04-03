#include <gtest/gtest.h>

#include "ev_loop.hpp"
#include "types.hpp"

namespace bh {

TEST(EvLoopTest, StartStopTest)
{
    startEvLoop([](unit) -> unit {
        stopEvLoop();
        return unit();
    });
}

TEST(EvLoopTest, PutTaskTest)
{
    startEvLoop([](unit) -> unit {
        future<unit> fut = pushToEvLoop<unit>([](unit) -> unit {
            stopEvLoop();
            return unit();
        });

        return unit();
    });
}

TEST(EvLoopTest, ThenTest)
{
    startEvLoop([](unit) -> unit {
        future<unit> fut = pushToEvLoop<u32>([](unit) -> u32 { return 42; }).then<unit>([](u32 res) -> unit {
            EXPECT_EQ(42, res);
            stopEvLoop();
            return unit();
        });

        return unit();
    });
}

void count(int counter)
{
    if (counter == 0) {
        stopEvLoop();
    } else {
        future<unit> fut = pushToEvLoop<unit>([counter = counter - 1](unit) -> unit { //
            count(counter);
            return unit();
        });
    }
}

TEST(EvLoopTest, CounterTest)
{
    startEvLoop([](unit) -> unit {
        count(10);
        return unit();
    });
}

}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
