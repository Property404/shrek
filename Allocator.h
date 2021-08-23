#pragma once
#include <cstddef>
#include <utility>
#include "common.h"

/// A memory allocator intended to be used to
/// allocate memory off Shrek's heap.
class Allocator {
    struct Node {
        size_t size;
        Node* next;
    };

    static constexpr size_t kAlignment = 8;
    static constexpr size_t kNodeHeaderSize =
        ALIGN_UP(sizeof(Allocator::Node), kAlignment);

    /// Some area of memory we're using as a heap.
    void* buffer;

    /// Size of buffer/heap in bytes.
    size_t buffer_size;

    /// Total number of allocations, for statistics/debugging purposes
    size_t allocations = 0;

    /// Insert new node into linked list.
    void add_node_after(void* pointer, size_t size, Node* prev_node);

    /// Make sure `pointer` is within bounds.
    void check_bounds(void* pointer) const;

    // Contains the mem-alloc implementation.
    void* allocate_internal(size_t size);

 public:
    Allocator(void* buffer, size_t buffer_size);
    // We definitely don't wnat to copy or move an allocator
    Allocator(const Allocator& other) = delete;
    Allocator operator=(const Allocator& other) = delete;

    /// Allocate `size` bytes of memory off Allocator's buffer.
    template <typename T>
    T* allocate(size_t size) {
        return static_cast<T*>(allocate_internal(size));
    }

    /// Construct object in place
    template <typename T, typename... Ts>
    T* construct(Ts&&... args) {
        // Okay, we're not really doing this in place. Just constructing and then moving to
        // the spot we allocated. The disadvantage here being of course we need a move constructor.
        // I'm not sure how to do otherwise, though.
        T object(std::forward<Ts>(args)...);
        T* pointer = static_cast<T*>(allocate_internal(sizeof(T)));
        *pointer = std::move(object);
        return pointer;
    }

    /// Free memory back to Allocator.
    void free(void* ptr);

    size_t getNumberOfAllocations() const {
        return allocations;
    }
};
