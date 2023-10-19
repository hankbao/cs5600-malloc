// allocator.h
// Allocator Interface definition
// Author: Hank Bao

#pragma once

#include <list>

#include "allocator.h"

class AllocatorBase : public Allocator {
   public:
    AllocatorBase(size_t base, size_t size, bool coalesce, ListOrder order)
        : Allocator{}, base_{base}, size_{size}, coalesce_{coalesce}, order_{order}, searched_{0}, freelist_{} {
        freelist_.emplace_front(base, size);
    }
    virtual ~AllocatorBase() = default;

    virtual auto free(Chunk chunk) -> void override;

    virtual auto last_searched() const -> size_t override { return searched_; }
    virtual auto print_status() -> void override;

   protected:
    const size_t base_;
    const size_t size_;
    const bool coalesce_;
    const ListOrder order_;

    size_t searched_;
    std::list<Chunk> freelist_;

   private:
    AllocatorBase(const AllocatorBase&) = delete;
    AllocatorBase& operator=(const AllocatorBase&) = delete;
};
