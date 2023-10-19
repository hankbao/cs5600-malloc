// allocator.h
// Allocator interface definition
// Author: Hank Bao

#pragma once

#include "chunk.h"

enum class ListOrder {
    InsertBack,
    InsertFront,
    AddrSort,
    SizeSortAsc,
    SizeSortDesc,
};

class Allocator {
   public:
    Allocator() = default;
    virtual ~Allocator() = default;

    virtual auto malloc(size_t size) -> Chunk = 0;
    virtual auto free(Chunk chunk) -> void = 0;

    virtual auto print_status() -> void = 0;

   private:
    Allocator(const Allocator&) = delete;
    Allocator& operator=(const Allocator&) = delete;
};
