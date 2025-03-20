#include <gtest/gtest.h>

#include "ev_loop.hpp"

namespace bh {

TEST(EvLoopTest, StartStopTest)
{
    ev_loop loop;

    loop.start([&loop]() { loop.stop(); });
}

TEST(EvLoopTest, PutTaskTest)
{
    ev_loop loop;

    loop.start([&loop]() { loop.push([&loop]() { loop.stop(); }); });
}

void count(ev_loop& loop, int counter)
{
    if (counter == 0) {
        loop.stop();
    } else {
        loop.push([&loop, counter = counter - 1]() { //
            count(loop, counter);
        });
    }
}

TEST(EvLoopTest, CounterTest)
{
    ev_loop loop;

    loop.start([&loop]() { //
        count(loop, 10);
    });
}

}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
