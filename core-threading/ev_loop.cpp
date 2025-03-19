#include "ev_loop.hpp"

#include "logging.hpp"

namespace bh {

void ev_loop::start(task_t task)
{
    std::unique_lock lock(init_mutex_);

    ev_loop_thread_ = std::thread([this, task = std::move(task)]() {
        std::unique_lock lock(init_mutex_);

        while (!stop_.load()) {
            if (task_queue_.empty()) {
                continue;
            } else {
                task_t task = task_queue_.front();
                task_queue_.pop();
                task();
            }
        }
    });

    thread_id_ = ev_loop_thread_.get_id();
    task_queue_.push(std::move(task));
    stop_ = false;
}

void ev_loop::push(task_t task)
{
    if (thread_id_ != std::this_thread::get_id()) {
        throw std::runtime_error("Wrong thread id in ev_loop::push()!!!");
    }

    task_queue_.push(std::move(task));
}

void ev_loop::stop()
{
    stop_ = true;
}

void ev_loop::join()
{
    if (thread_id_ == std::this_thread::get_id()) {
        throw std::runtime_error("Wrong thread id in ev_loop::join()!!!");
    }

    ev_loop_thread_.join();
}

}
