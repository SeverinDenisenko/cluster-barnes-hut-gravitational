#pragma once

#include <cstddef>
#include <utility>

#include "task.hpp"

namespace bh {

template <typename T>
class future;

template <typename T>
class promice;

template <typename T>
std::pair<future<T>, promice<T>> create_futue_promice_pair() noexcept;

template <typename T>
struct future_promice_control_block {
    size_t refcount { 0 };
    bool resolved { false };
    T value {};
    task_t<unit, T> then_action {};
};

template <typename T>
class promice {
public:
    ~promice()
    {
        if (control_block_ == nullptr) {
            return;
        }

        --control_block_->refcount;
        if (control_block_->refcount == 0) {
            delete control_block_;
        }
    }

    promice(const promice&) = delete;
    promice()               = delete;

    promice(promice&& other)
    {
        control_block_       = other.control_block_;
        other.control_block_ = nullptr;
    }

    void resolve(T value)
    {
        control_block_->resolved = true;
        control_block_->value    = std::move(value);
        if (control_block_->then_action) {
            control_block_->then_action(control_block_->value);
            control_block_->then_action = {};
        }
    }

    template <typename U>
    friend std::pair<future<U>, promice<U>> create_futue_promice_pair() noexcept;

private:
    promice(future_promice_control_block<T>* control_block) noexcept
        : control_block_(control_block)
    {
        ++control_block_->refcount;
    }

    future_promice_control_block<T>* control_block_;
};

template <typename T>
class future {
public:
    ~future()
    {
        if (control_block_ == nullptr) {
            return;
        }

        --control_block_->refcount;
        if (control_block_->refcount == 0) {
            delete control_block_;
        }
    }

    future(const future&) = delete;
    future()              = delete;

    future(future&& other)
    {
        control_block_       = other.control_block_;
        other.control_block_ = nullptr;
    }

    template <typename U>
    friend std::pair<future<U>, promice<U>> create_futue_promice_pair() noexcept;

    template <typename Ret>
    future<Ret> then(task_t<Ret, T> task);

private:
    future(future_promice_control_block<T>* control_block) noexcept
        : control_block_(control_block)
    {
        ++control_block_->refcount;
    }

    future_promice_control_block<T>* control_block_;
};

template <typename T>
std::pair<future<T>, promice<T>> create_futue_promice_pair() noexcept
{
    future_promice_control_block<T>* contol_block = new future_promice_control_block<T>();
    future<T> fut(contol_block);
    promice<T> prom(contol_block);
    return std::pair<future<T>, promice<T>>(std::move(fut), std::move(prom));
}

template <typename Arg>
template <typename Ret>
future<Ret> future<Arg>::then(task_t<Ret, Arg> task)
{
    auto [fut, prom] = create_futue_promice_pair<Ret>();

    control_block_->then_action = [prom = std::move(prom), task = std::move(task)](Arg arg) mutable -> unit {
        prom.resolve(task(std::move(arg)));
        return unit();
    };

    return std::move(fut);
}

};
