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

    // move to the next chunk
    if (last_ >= freelist_.size()) {
        last_ = 0;
    }

    std::printf("Next-fit: search from index %lu\n", freelist_.size() > 1 ? last_ + 1 : 0);

    auto it = freelist_.begin();
    for (size_t counter = last_ + 1; counter > 0; --counter) {
        ++it;  // move to the next position
        if (it == freelist_.end()) {
            it = freelist_.begin();
        }
    }

    // search for the next fit
    auto fit = freelist_.end();
    for (size_t counter = freelist_.size(); counter > 0; --counter) {
        ++searched_;
        ++last_;
        if (last_ >= freelist_.size()) {
            last_ = 0;
        }

        if (it->size() >= size) {
            fit = it;
            break;
        }

        ++it;
        if (it == freelist_.end()) {
            it = freelist_.begin();
        }
    }

    if (fit != freelist_.end()) {
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
