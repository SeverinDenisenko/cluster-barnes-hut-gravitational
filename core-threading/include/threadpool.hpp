#pragma once

#include <functional>
#include <list>
#include <mutex>
#include <thread>

namespace bh {

class threadpool {
public:
    using task_t = std::function<void(void)>;

    threadpool();

    void put(task_t task);
    size_t threads();
    void wait();
    void join();

private:
    bool has_task();

    size_t threads_count_;
    size_t round_robin_counter_;
    std::vector<std::list<task_t>> tasks_;
    std::vector<std::mutex> mutexes_;
    std::vector<size_t> stop_signals_;
    std::vector<std::thread> threads_;
};

}
