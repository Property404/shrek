#include "common.h"
#include "Allocator.h"
     

Allocator::Allocator(void* buffer, size_t buffer_size) {
    initialize(buffer, buffer_size);
}

void Allocator::initialize(void* buffer, size_t buffer_size) {
    if (buffer == nullptr || buffer_size == 0) {
        panic("Invalid arguments for Allocator::initialize");
    }

    this->buffer = buffer;
    this->buffer_size = buffer_size;
    Node* head = static_cast<Node*>(buffer);
    head->size = kNodeHeaderSize;
    head->next = nullptr;
}

void Allocator::add_node_after(void* pointer, size_t size, Node* prev_node) {
    if(size == 0) {
        panic("Zero-sized allocator nodes are forbidden");
    }

    check_bounds(pointer);
    check_bounds(prev_node);

    Node* new_node = static_cast<Node*>(pointer);
    new_node->size = size;
    new_node->next = prev_node->next;
    prev_node->next = new_node;

}

void Allocator::check_bounds(void* pointer) const {
    if (buffer == nullptr) {
        panic("Allocator has null buffer!");
    }

    uintptr_t start = reinterpret_cast<uintptr_t>(buffer);
    uintptr_t end = start + buffer_size;
    if(reinterpret_cast<uintptr_t>(pointer) < start ||
        reinterpret_cast<uintptr_t>(pointer) >= end)
    {
        panic("Out of bounds!");
    }
}

void* Allocator::allocate_internal(size_t size) {
    if(size == 0) {
        panic("Cannot allocate object of size zero");
    }
    if(buffer == nullptr) {
        panic("Allocator has null buffer!");
    }

    size = ALIGN_UP(size + kNodeHeaderSize, kAlignment);

    Node* previous_node = nullptr;
    Node* node = static_cast<Node*>(buffer);
    void* prospect = buffer;
    while(true) {
        if (reinterpret_cast<uintptr_t>(prospect) + size > reinterpret_cast<uintptr_t>(node)) {
            prospect = (uint8_t*)node + node->size;
        } else {
            break;
        }
        previous_node = node;
        node = node->next;
        if(node == nullptr) {
            break;
        }
        check_bounds(node);
    }
    if ((uintptr_t)prospect + size >= ((uintptr_t)buffer + buffer_size)) {
        panic("Out of memory");
    }
    add_node_after(prospect, size, previous_node);
    return (uint8_t*)prospect + kNodeHeaderSize;
}

void Allocator::free(void* ptr) {
    ptr = (uint8_t*)ptr - kNodeHeaderSize;
    check_bounds(ptr);

    Node* previous_node = nullptr;
    Node* node = static_cast<Node*>(buffer);
    while(true) {
        if (node == static_cast<Node*>(ptr)) {
            if(previous_node) {
                previous_node->next = node->next;
            }
            return;
        }
        previous_node = node;
        node = node->next;
        if(node == nullptr) {
            break;
        }
        check_bounds(node);
    }

    panic("Attempted to free non-allocated block");
}
