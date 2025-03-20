#include "ev_loop.hpp"

namespace bh {

void ev_loop::start(task_t task)
{
    task_queue_.push(std::move(task));
    stop_ = false;

    while (!stop_) {
        if (task_queue_.empty()) {
            continue;
        } else {
            task_t task = task_queue_.front();
            task_queue_.pop();
            task();
        }
    }
}

void ev_loop::push(task_t task)
{
    task_queue_.push(std::move(task));
}

void ev_loop::stop()
{
    stop_ = true;
}

}
