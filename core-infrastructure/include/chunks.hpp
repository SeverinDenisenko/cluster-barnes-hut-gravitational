#pragma once

#include "types.hpp"

namespace bh {

struct chunk {
    u32 begin;
    u32 end;
};

inline array<chunk> make_chunks(u32 jobs, u32 workers)
{
    array<chunk> chunks(workers);

    u32 base_chunk_size = jobs / workers;
    u32 remainder       = jobs % workers;

    u32 current = 0;
    for (u32 worker = 0; worker < workers; ++worker) {
        u32 additional = (worker < remainder) ? 1 : 0;
        u32 chunk_size = base_chunk_size + additional;

        chunks[worker] = chunk { .begin = chunk_size * worker, .end = current + chunk_size };

        current += chunk_size;
    }

    return chunks;
}

}
