// allocator_next.h
// First-fit allocator definition
// Author: Hank Bao

#pragma once

#include "allocator_base.h"

class AllocatorNext : public AllocatorBase {
   public:
        : AllocatorBase{base, size, coalesce, order}, last_{freelist_.begin()} {};
    AllocatorNext(size_t base, size_t size, bool coalesce, ListOrder order)
    virtual ~AllocatorNext() = default;

    virtual auto malloc(size_t size) -> Chunk override;

   private:
    std::list<Chunk>::iterator last_;

    AllocatorNext(const AllocatorNext&) = delete;
    AllocatorNext& operator=(const AllocatorNext&) = delete;
};
