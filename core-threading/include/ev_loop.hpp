#pragma once

#include <atomic>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

namespace bh {

class ev_loop {
public:
    using task_t = std::function<void(void)>;

    ev_loop() = default;

    void start(task_t task);
    void push(task_t task);
    void stop();
    void join();

private:
    std::mutex init_mutex_;
    std::thread ev_loop_thread_;
    std::thread::id thread_id_;

    std::queue<task_t> task_queue_;
    std::atomic_bool stop_;
};

}
