// allocator_worst.h
// Worst-fit allocator definition
// Author: Hank Bao

#pragma once

#include "allocator_base.h"

class AllocatorWorst : public AllocatorBase {
   public:
    AllocatorWorst(size_t base, size_t size, bool coalesce, ListOrder order)
        : AllocatorBase{base, size, coalesce, order} {};
    virtual ~AllocatorWorst() = default;

    virtual auto malloc(size_t size) -> Chunk override;

   private:
    AllocatorWorst(const AllocatorWorst&) = delete;
    AllocatorWorst& operator=(const AllocatorWorst&) = delete;
};
