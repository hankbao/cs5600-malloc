// chunk.h
// Chunk represents a block of memory with base address and size.
// Author: Hank Bao

#pragma once

#include <cstddef>

class Chunk {
   public:
    Chunk(size_t base, size_t size) : base_{base}, size_{size} {}
    ~Chunk() {}

    auto base() const -> size_t { return base_; }
    auto size() const -> size_t { return size_; }
    auto is_null() const -> bool { return base_ == 0 && size_ == 0; }

    // shrink from the head of chunk
    auto shrink(size_t size) -> void {
        base_ += size;
        size_ -= size;
    }

    // expand from the tail of chunk
    auto expand(size_t size) -> void {
        size_ += size;
    }

   private:
    size_t base_;
    size_t size_;
};

inline auto operator==(const Chunk& a, const Chunk& b) -> bool {
    return a.base() == b.base() && a.size() == b.size();
}
