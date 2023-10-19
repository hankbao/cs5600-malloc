// allocator_best.cc
// Best-fit allocator implementation
// Author: Hank Bao

#include "allocator_best.h"

// Find the smallest free chunk to fit the given size.
auto AllocatorBest::malloc(size_t size) -> Chunk {
    if (0 == size) {
        return Chunk{0, 0};  // {0, 0} as null
    }
    if (freelist_.empty()) {
        return Chunk{0, 0};
    }
    searched_ = 0;

    // search for the smallest fit
    auto fit = freelist_.end();
    for (auto it = freelist_.begin(); it != freelist_.end(); ++it) {
        ++searched_;

        if (it->size() >= size) {
            if (fit == freelist_.end()) {
                fit = it;
            } else if (it->size() < fit->size()) {
                fit = it;
            }
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
