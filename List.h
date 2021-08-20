#pragma once
#include <iterator>
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
    List operator=(const List& other) = delete;

    List(List&& other) {
        this->head_ = other.head_;
        this->tail_ = other.tail_;
        this->size_ = other.size_;
        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;
    }

    List& operator=(List&& other) {
        this->head_ = other.head_;
        this->tail_ = other.tail_;
        this->size_ = other.size_;
        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;
        return *this;
    }

    /// Iterator over Node's.
    struct Iterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using NodePointer = const Node*;
        using NodeReference = const Node&;
        using ContentPointer = T*;
        using ContentReference = const T&;

        explicit Iterator(NodePointer pointer) {
            this->pointer = pointer;
        }

        ContentReference operator*() {
            return pointer->data_;
        }

        // Pointer operator->(){return &(pointer->content);}

        Iterator& operator++() {
            pointer = pointer->next_;
            return *this;
        }

        friend bool operator==(Iterator& a, const Iterator& b) {
            return a.pointer == b.pointer;
        }

        friend bool operator!=(Iterator& a, const Iterator& b) {
            return a.pointer != b.pointer;
        }

     private:
            NodePointer pointer;
    };

    Iterator begin() const {
        return Iterator(head_);
    }

    Iterator end() const {
        return Iterator(nullptr);
    }

    /// Add new item `data` to List.
    T& push_back(auto&& data) {
        Node* new_node = allocator.allocate<Node>(sizeof(Node));
        new_node->data_ = std::forward<T>(data);
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

    template<typename... Ts>
    T& emplace_back(Ts&&... args) {
        T object(std::forward<Ts>(args)...);
        return push_back(std::move(object));
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
        return head_->data_;
    }

    /// Return the last element of the list.
    T& back() const {
        return tail_->data_;
    }

    size_t size() const {
        return size_;
    }
};

