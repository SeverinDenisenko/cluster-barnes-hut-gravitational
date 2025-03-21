#pragma once

#include <iterator>
#include <stdexcept>
#include <type_traits>

#include "fmt/format.h"

#include "types.hpp"

namespace bh {

class cluster_transport {
public:
    cluster_transport() = default;

    cluster_transport(const cluster_transport&) = delete;
    cluster_transport(cluster_transport&&)      = delete;

    template <typename T>
        requires(std::is_trivially_copyable_v<T>)
    void send_struct(T msg, u32 node)
    {
        send(&msg, sizeof(T), node, message_type::STRUCT);
    }

    template <typename T>
        requires(std::is_trivially_copyable_v<T>)
    T receive_struct(u32 node)
    {
        T msg;
        receive(&msg, sizeof(T), node, message_type::STRUCT);
        return msg;
    }

    template <typename T>
        requires(std::is_trivially_copyable_v<T>)
    void send_array(const array<T>& msg, u32 node)
    {
        send((void*)msg.data(), msg.size() * sizeof(T), node, message_type::ARRAY);
    }

    template <typename T>
        requires(std::is_trivially_copyable_v<T>)
    array<T> receive_array(u32 node)
    {
        u32 count = msg_size(node, message_type::ARRAY) / sizeof(T);
        array<T> msg(count);
        receive(msg.data(), count * sizeof(T), node, message_type::ARRAY);
        return msg;
    }

    template <typename T>
        requires(std::is_trivially_copyable_v<T>)
    void send_array(array<T>::iterator begin, array<T>::iterator end, u32 node)
    {
        send(&(*begin), std::distance(begin, end) * sizeof(T), node, message_type::ARRAY);
    }

    template <typename T>
        requires(std::is_trivially_copyable_v<T>)
    void receive_array(array<T>::iterator begin, array<T>::iterator end, u32 node)
    {
        u32 count = msg_size(node, message_type::ARRAY) / sizeof(T);
        if (std::distance(begin, end) != count) {
            throw std::runtime_error(fmt::format(
                "Recived wrong amount in cluster_transport::receive_array(): recv={}, expected={}",
                count,
                std::distance(begin, end)));
        }
        receive(&(*begin), count * sizeof(T), node, message_type::ARRAY);
    }

private:
    enum class message_type : int {
        ARRAY,
        STRUCT,
    };

    void send(void* buff, u32 size, u32 node, message_type type);
    u32 msg_size(u32 node, message_type type);
    void receive(void* buff, u32 size, u32 node, message_type type);
};

}
