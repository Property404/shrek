#include "common.h"
#include <cstddef>

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

    /// Insert new node into linked list.
    void add_node_after(void* pointer, size_t size, Node* prev_node);

    /// Make sure `pointer` is within bounds.
    void check_bounds(void* pointer) const;

    // Contains the mem-alloc implementation.
    void* allocate_internal(size_t size);

    public:

    Allocator(void* buffer, size_t buffer_size);

    /// Called by the constructor. This is
    /// needed because BSS is cleared.
    void initialize(void* buffer, size_t buffer_size);

    /// Allocate some memory off Allocator's buffer.
    template <typename T>
    T* allocate(size_t size) {
        return static_cast<T*>(allocate_internal(size));
    }

    /// Free memory back to Allocator.
    void free(void* ptr);
};
