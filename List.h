#pragma once
#include <utility>
#include "memory.h"

/// Linked list.
template <typename T>
class List {
    struct Node {
        T data_;
        Node* next_;
    };

    Node* head_;
    Node* tail_;
    size_t size_;

 public:
    List() {
        head_ = nullptr;
        tail_ = nullptr;
        size_ = 0;
    }

    ~List() {
        Node* node = head_;
        while (node != nullptr) {
            Node* next_node = node->next_;

            // No delete[] yet so do this explicitly
            node->~Node();
            allocator.free(node);

            node = next_node;
        }
        head_ = nullptr;
        tail_ = nullptr;
        size_ = 0;
    }

    List(const List& other) = delete;
    List(List&& other) = delete;
    List operator=(const List& other) = delete;

    /// Add new item containing a copy of `data` to List.
    T& push_back(T data) {
        return emplace_back(std::move(data));
    }

    /// Add new item containing `data` to List.
    T& emplace_back(T&& data) {
        Node* new_node = allocator.allocate<Node>(sizeof(Node));
        new_node->data_ = std::move(data);
        new_node->next_ = nullptr;

        if (tail_ == nullptr) {
            head_ = new_node;
            tail_ = new_node;
        } else {
            tail_->next_ = new_node;
            tail_ = new_node;
        }

        size_++;
        return new_node->data_;
    }

    /// Get element from list at index `index`.
    /// This is O(N). If you're using this, you probably want a
    /// contiguous container instead.
    T& get(size_t index) const {
        if (index >= size_) {
            panic("Can't access non-existent element in List");
        }

        size_t i = 0;
        for (Node* node = head_ ;; node = node->next_) {
            if (node == nullptr) {
                panic("Encountered null node in List @0x%x", i);
            }
            if (i == index) {
                return node->data_;
            }
            i++;
        }
    }

    /// Return the first element of the list.
    T& front() const {
        return head_->data;
    }

    /// Return the last element of the list.
    T& back() const {
        return tail_->data;
    }

    size_t size() const {
        return size_;
    }
};

