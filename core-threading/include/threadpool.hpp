#pragma once

#include <functional>
#include <list>
#include <mutex>
#include <thread>

namespace bh {

class threadpool {
public:
    using task_t = std::function<void(void)>;

    threadpool()
        : threads_count_(std::thread::hardware_concurrency())
        , round_robin_counter_(0)
        , tasks_(threads_count_)
        , mutexes_(threads_count_)
        , stop_signals_(threads_count_, 0)
    {
        for (size_t thread = 0; thread < threads_count_; ++thread) {
            threads_.emplace_back(std::thread([this, id = thread]() {
                task_t task = {};

                while (true) {
                    {
                        // check if we have been stoped
                        std::unique_lock lk(mutexes_[id]);
                        if (tasks_[id].empty() && stop_signals_[id] == 1) {
                            return;
                        }

                        // if queue is empty -- pass
                        // if there is task -- extract it
                        if (tasks_[id].empty()) {
                            continue;
                        } else {
                            task = tasks_[id].front();
                            tasks_[id].pop_front();
                        }
                    }

                    if (task) {
                        task();
                        task = {};
                    }
                }
            }));
        }
    }

    void put(task_t task)
    {
        size_t thread = round_robin_counter_++ % threads_count_;

        std::unique_lock lk(mutexes_[thread]);
        tasks_[thread].push_back(std::move(task));
    }

    size_t threads()
    {
        return threads_count_;
    }

    void wait()
    {
        while (has_task()) {
            continue;
        }
    }

    void join()
    {
        for (size_t thread = 0; thread < threads_count_; ++thread) {
            std::unique_lock lk(mutexes_[thread]);
            stop_signals_[thread] = 1;
        }

        for (auto& thread : threads_) {
            thread.join();
        }
    }

private:
    bool has_task()
    {
        for (size_t thread = 0; thread < threads_count_; ++thread) {
            std::unique_lock lk(mutexes_[thread]);
            if (!tasks_[thread].empty()) {
                return true;
            }
        }

        return false;
    }

    size_t threads_count_;
    size_t round_robin_counter_;
    std::vector<std::list<task_t>> tasks_;
    std::vector<std::mutex> mutexes_;
    std::vector<size_t> stop_signals_;
    std::vector<std::thread> threads_;
};

}
