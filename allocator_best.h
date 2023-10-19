// allocator_best.h
// Best-fit allocator definition
// Author: Hank Bao

#pragma once

#include "allocator_base.h"

class AllocatorBest : public AllocatorBase {
   public:
    AllocatorBest(size_t base, size_t size, bool coalesce, ListOrder order = ListOrder::AddrSort)
        : AllocatorBase{base, size, coalesce, order} {};
    virtual ~AllocatorBest() = default;

    virtual auto malloc(size_t size) -> Chunk override;

   private:
    AllocatorBest(const AllocatorBest&) = delete;
    AllocatorBest& operator=(const AllocatorBest&) = delete;
};
