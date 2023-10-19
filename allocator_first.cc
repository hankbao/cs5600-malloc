// allocator_fisrt.cc
// First-fit allocator implementation
// Author: Hank Bao

#include "allocator_first.h"

// Find the fisrt free chunk to fit the given size.
auto AllocatorFirst::malloc(size_t size) -> Chunk {
    if (0 == size) {
        return Chunk{0, 0};  // {0, 0} as null
    }

    // search for the first fit
    auto fit = freelist_.end();
    for (auto it = freelist_.begin(); it != freelist_.end(); ++it) {
        if (it->size() >= size) {
            fit = it;
            break;
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
