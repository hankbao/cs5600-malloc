// allocator_next.cc
// Next-fit allocator implementation
// Author: Hank Bao

#include "allocator_next.h"

// Find the next free chunk to fit the given size according to last search.
auto AllocatorNext::malloc(size_t size) -> Chunk {
    if (0 == size) {
        return Chunk{0, 0};  // {0, 0} as null
    }
    if (freelist_.empty()) {
        return Chunk{0, 0};
    }
    searched_ = 0;

    // search for the next fit
    auto fit = freelist_.end();
    ++last_;  // move to the next chunk
    auto it = last_;
    do {
        if (it == freelist_.end()) {
            it = freelist_.begin();
            continue;
        }

        ++searched_;
        if (it->size() >= size) {
            fit = it;
            break;
        }

        ++it;
    } while (it != last_);  // one round of search

    if (fit != freelist_.end()) {
        last_ = fit;  // keep the last position

        // perfect fit
        if (fit->size() == size) {
            auto c = *fit;
            freelist_.erase(fit);
            return c;
        } else {
            Chunk c{fit->base(), size};
            fit->shrink(size);
            return c;
        }
    } else {
        // search failed
        return Chunk{0, 0};
    }
}
