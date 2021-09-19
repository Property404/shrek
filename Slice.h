#pragma once
#include <iterator>
#include <type_traits>
#include <functional>
#include "memory.h"

/// Contiguous array over non-owned data. Allows an optional ScalerTransform
/// parameter to transform the data when read without modifying the underlying
/// buffer
template <typename T, std::function<T(T t)>* ScalerTransform = nullptr>
class Slice final {
    T* data_ = nullptr;
    // How many items
    size_t size_ = 0;

 public:
    Slice(T* data, size_t size) {
        data_ = data;
        size_ = size;
    }

    ~Slice() {
        data_ = nullptr;
        size_ = 0;
    }

    Slice(const Slice& other) = delete;
    Slice operator=(const Slice& other) = delete;
    Slice(Slice&& other) = default;
    Slice& operator=(Slice&& other) = default;

    /// Iterator over the internal array
    template<typename SliceType>
    struct Iterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::int64_t;

        explicit Iterator(SliceType* slice, size_t index) {
            this->slice = slice;
            this->index = index;
        }

        std::conditional_t<ScalerTransform == nullptr, const T&, T>
        operator*() const {
            return (*slice)[index];
        }

        Iterator& operator++() {
            index++;
            return *this;
        }

        friend bool operator==(Iterator& a, const Iterator& b) {
            return a.index == b.index;
        }

        friend bool operator!=(Iterator& a, const Iterator& b) {
            return a.index != b.index;
        }

     private:
            size_t index;
            SliceType* slice;
    };

    auto begin() const {
        return Iterator<typename std::remove_reference<decltype(*this)>::type>(this, 0);
    }

    auto end() const {
        return Iterator<typename std::remove_reference<decltype(*this)>::type>(this, size_);
    }

    /// Random access
    template<typename U = T>
    std::enable_if_t<ScalerTransform == nullptr, U&> operator[](size_t index) {
        if (index >= size_) {
            panic("Can't access non-existent element(0x%x) in Slice of size (0x%x)", index, size_);
        }
        return data_[index];
    }

    /// Read-only random access. Will transform data on access if
    /// ScalerTransform is defined.
    std::conditional_t<ScalerTransform == nullptr, const T&, T> operator[](size_t index) const {
        if (index >= size_) {
            panic("Can't access non-existent element(0x%x) in Slice of size (0x%x)", index, size_);
        }

        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Waddress"
        if constexpr (ScalerTransform == nullptr) {
        #pragma GCC diagnostic pop
            return data_[index];
        } else {
            return (*ScalerTransform)(data_[index]);
        }
    }

    /// @return The number of items of the array
    size_t size() const {
        return size_;
    }
};

