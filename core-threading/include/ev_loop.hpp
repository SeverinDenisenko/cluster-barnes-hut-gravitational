#pragma once

#include <functional>
#include <queue>

namespace bh {

class ev_loop {
public:
    using task_t = std::function<void(void)>;

    ev_loop() = default;

    void start(task_t task);
    void push(task_t task);
    void stop();

private:
    std::queue<task_t> task_queue_;
    bool stop_;
};

}
