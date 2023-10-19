// allocator_worst.cc
// Worst-fit allocator implementation
// Author: Hank Bao

#include "allocator_worst.h"

// Find the biggest free chunk to fit the given size.
auto AllocatorWorst::malloc(size_t size) -> Chunk {
    searched_ = 0;

    if (0 == size) {
        return Chunk{0, 0};  // {0, 0} as null
    }

    // search for the biggest fit
    auto fit = freelist_.end();
    for (auto it = freelist_.begin(); it != freelist_.end(); ++it) {
        ++searched_;

        if (it->size() >= size) {
            if (fit == freelist_.end()) {
                fit = it;
            } else if (it->size() > fit->size()) {
                fit = it;
            }
        }
    }

    if (fit != freelist_.end()) {
        if (fit->size() == size) {  // perfect fit
            auto c = *fit;
            freelist_.erase(fit);
            return c;
        } else {  // biggest fit
            Chunk c{fit->base(), size};
            fit->shrink(size);
            return c;
        }
    } else {
        return Chunk{0, 0};  // search failed
    }
}
