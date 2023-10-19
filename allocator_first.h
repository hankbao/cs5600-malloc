// allocator_first.h
// First-fit allocator definition
// Author: Hank Bao

#pragma once

#include "allocator_base.h"

class AllocatorFirst : public AllocatorBase {
   public:
    AllocatorFirst(size_t base, size_t size, bool coalesce, ListOrder order = ListOrder::AddrSort)
        : AllocatorBase{base, size, coalesce, order} {};
    virtual ~AllocatorFirst() = default;

    virtual auto malloc(size_t size) -> Chunk override;

   private:
    AllocatorFirst(const AllocatorFirst&) = delete;
    AllocatorFirst& operator=(const AllocatorFirst&) = delete;
};
