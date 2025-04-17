#pragma once

#include "chunks.hpp"
#include "model.hpp"
#include "solver.hpp"
#include "types.hpp"

namespace bh {

enum class cluster_message_type : u32 {
    chunk         = 1,
    solver_params = 2,
    points        = 3,
    stop          = 4,
};

struct chunk_message {
    static constexpr cluster_message_type msg_type = cluster_message_type::chunk;

    chunk chunk_;

    void parce(const void* buffer)
    {
        chunk_ = *reinterpret_cast<const chunk*>(buffer);
    }

    size_t size()
    {
        return sizeof(chunk);
    }

    void serialize(void* buffer)
    {
        *reinterpret_cast<chunk*>(buffer) = chunk_;
    }
};

struct solver_params_message {
    static constexpr cluster_message_type msg_type = cluster_message_type::solver_params;

    solver_params params_;

    void parce(const void* buffer)
    {
        params_ = *reinterpret_cast<const solver_params*>(buffer);
    }

    size_t size()
    {
        return sizeof(solver_params);
    }

    void serialize(void* buffer)
    {
        *reinterpret_cast<solver_params*>(buffer) = params_;
    }
};

struct points_message {
    static constexpr cluster_message_type msg_type = cluster_message_type::points;

    array<point_t>& points_;

    void parce(const void* buffer)
    {
        memcpy(points_.data(), buffer, points_.size() * sizeof(point_t));
    }

    size_t size()
    {
        return sizeof(point_t) * points_.size();
    }

    void serialize(void* buffer)
    {
        memcpy(buffer, points_.data(), points_.size() * sizeof(point_t));
    }
};

struct stop_message {
    static constexpr cluster_message_type msg_type = cluster_message_type::stop;

    void parce(const void*)
    {
    }

    size_t size()
    {
        return 1;
    }

    void serialize(void*)
    {
    }
};

}
