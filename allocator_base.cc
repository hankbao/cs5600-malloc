// allocator.cc
// Base allocator implementation
// Author: Hank Bao

#include "allocator_base.h"

#include <algorithm>
#include <cstdio>

// Currently we don't consider invalid chuck
auto AllocatorBase::free(Chunk chunk) -> void {
    switch (order_) {
        case ListOrder::InsertBack:
            freelist_.push_back(chunk);
            break;
        case ListOrder::InsertFront:
            freelist_.push_front(chunk);
            break;
        case ListOrder::AddrSort: {
            auto it = std::lower_bound(freelist_.begin(), freelist_.end(), chunk,
                                       [](const Chunk& a, const Chunk& c) {
                                           return a.base() < c.base();
                                       });
            // --it means one position before it
            // inserting is okay even if it is end()
            freelist_.insert(--it, chunk);
        } break;
        case ListOrder::SizeSortAsc: {
            auto it = std::lower_bound(freelist_.begin(), freelist_.end(), chunk,
                                       [](const Chunk& a, const Chunk& c) {
                                           return a.size() < c.size();
                                       });
            freelist_.insert(--it, chunk);
        } break;
        case ListOrder::SizeSortDesc: {
            auto it = std::lower_bound(freelist_.begin(), freelist_.end(), chunk,
                                       [](const Chunk& a, const Chunk& c) {
                                           return a.size() > c.size();
                                       });
            freelist_.insert(--it, chunk);
        }
    }

    if (coalesce_) {
        for (auto current = freelist_.begin(); current != freelist_.end(); ++current) {
            for (auto it = freelist_.begin(); it != freelist_.end();) {
                if (it == current) continue;

                if (current->base() + current->size() == it->base()) {
                    current->expand(it->size());
                    freelist_.erase(it);
                    // as we merged two chunks together, we need to start over
                    // as any previous chunk could be coalescable now
                    it = freelist_.begin();
                } else {
                    ++it;
                }
            }
        }
    }
}

auto AllocatorBase::print_status() -> void {
    std::printf("Free List [ Size: %lu ]: ", freelist_.size());
    for (auto& chunk : freelist_) {
        std::printf("[ Base: %lu, Size: %lu ] ", chunk.base(), chunk.size());
    }
    std::puts("\n");
}
