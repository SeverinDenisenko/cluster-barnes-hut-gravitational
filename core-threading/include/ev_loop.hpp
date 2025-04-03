#pragma once

#include <list>

#include "future.hpp"
#include "task.hpp"

namespace bh {

class ev_loop {
public:
    ev_loop() = default;

    void start(task_t<unit, unit> task)
    {
        task_queue_.push_back(std::move(task));
        stop_ = false;

        while (!stop_) {
            if (task_queue_.empty()) {
                continue;
            } else {
                task_queue_.front()(unit());
                task_queue_.pop_front();
            }
        }
    }

    template <typename Ret>
    future<Ret> push(task_t<Ret, unit> task)
    {
        auto [fut, prom] = create_futue_promice_pair<Ret>();
        task_queue_.push_back([task = std::move(task), prom = std::move(prom)](unit) mutable -> unit {
            prom.resolve(task(unit()));
            return unit();
        });
        return std::move(fut);
    }

    void stop()
    {
        stop_ = true;
    }

private:
    std::list<task_t<unit, unit>> task_queue_;
    bool stop_;
};

inline ev_loop g_event_loop;

inline void startEvLoop(task_t<unit, unit> task)
{
    g_event_loop.start(std::move(task));
}

inline void stopEvLoop()
{
    g_event_loop.stop();
}

template <typename Ret>
inline future<Ret> pushToEvLoop(task_t<Ret, unit> task)
{
    return g_event_loop.push<Ret>(std::move(task));
}

}
