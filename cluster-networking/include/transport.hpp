#pragma once

#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "fmt/format.h"

#include "ev_loop.hpp"
#include "logging.hpp"
#include "types.hpp"

namespace bh {

class cluster_transport {
public:
    cluster_transport() = default;

    cluster_transport(const cluster_transport&) = delete;
    cluster_transport(cluster_transport&&)      = delete;

    template <typename Message>
    void poll_message(u32 node, task_t<unit, Message> handler, Message recv_msg, bool recurring = false)
    {
        if (can_recive(node, std::to_underlying(recv_msg.msg_type))) {
            size_t size = msg_size(node, std::to_underlying(recv_msg.msg_type));
            array<std::byte> buffer(size);

            receive(buffer.data(), size, node, std::to_underlying(recv_msg.msg_type));
            recv_msg.parce(buffer.data());

            handler(recv_msg);
            if (recurring) {
                pushToEvLoop<unit>([this, node, handler = std::move(handler), recv_msg, recurring](unit) -> unit {
                    poll_message(node, std::move(handler), recv_msg, recurring);

                    return unit();
                });
            }
        } else {
            pushToEvLoop<unit>([this, node, handler = std::move(handler), recv_msg, recurring](unit) -> unit {
                poll_message(node, std::move(handler), recv_msg, recurring);

                return unit();
            });
        }
    }

    template <typename Message>
    void add_handler(u32 node, task_t<unit, Message> handler, Message recv_message, bool recurring = false)
    {
        pushToEvLoop<unit>([this, node, handler = std::move(handler), recv_message, recurring](unit) -> unit {
            poll_message(node, std::move(handler), recv_message, recurring);

            return unit();
        });
    }

    template <typename Message>
    void send_message(u32 node, Message msg)
    {
        array<std::byte> data(msg.size());
        msg.serialize(data.data());
        send(data.data(), msg.size(), node, std::to_underlying(msg.msg_type));
    }

    template <typename T>
        requires(std::is_trivially_copyable_v<T>)
    void send_array(const array<T>& msg, u32 node, u32 msg_id)
    {
        send((void*)msg.data(), msg.size() * sizeof(T), node, msg_id);
    }

    template <typename T>
        requires(std::is_trivially_copyable_v<T>)
    array<T> receive_array(u32 node, u32 msg_id)
    {
        u32 count = msg_size(node, msg_id) / sizeof(T);
        array<T> msg(count);
        receive(msg.data(), count * sizeof(T), node, msg_id);
        return msg;
    }

    template <typename T>
        requires(std::is_trivially_copyable_v<T>)
    void send_array(array<T>::iterator begin, array<T>::iterator end, u32 node, u32 msg_id)
    {
        send(&(*begin), std::distance(begin, end) * sizeof(T), node, msg_id);
    }

    template <typename T>
        requires(std::is_trivially_copyable_v<T>)
    void receive_array(array<T>::iterator begin, array<T>::iterator end, u32 node, u32 msg_id)
    {
        u32 count = msg_size(node, msg_id) / sizeof(T);
        if (std::distance(begin, end) != count) {
            throw std::runtime_error(fmt::format(
                "Recived wrong amount in cluster_transport::receive_array(): recv={}, expected={}",
                count,
                std::distance(begin, end)));
        }
        receive(&(*begin), count * sizeof(T), node, msg_id);
    }

private:
    void send(void* buff, u32 size, u32 node, u32 type);
    u32 msg_size(u32 node, u32 type);
    void receive(void* buff, u32 size, u32 node, u32 type);
    bool can_recive(u32 node, u32 msg_id);
};

}
