#pragma once

#include <iostream>
#include <array>
#include <optional>
#include <stdexcept>

namespace chai
{
    template <typename T, size_t Capacity>
    class RingBuffer
    {
    private:
        std::array<T, Capacity> buffer_;
        alignas(64) std::atomic<size_t> head_ = 0; // Index for next pop
        alignas(64) std::atomic<size_t> tail_ = 0; // Index for next push
        alignas(64) std::atomic<size_t> size_ = 0; // Current number of elements
    public:
        RingBuffer()
        {
            if constexpr (Capacity == 0) {
                throw std::invalid_argument("Capacity must be greater than 0");
            }
        }

        // Push element; overwrites oldest element if full
        void push(const T& item)
        {
            buffer_[tail_] = item;
            tail_ = (tail_ + 1) % Capacity;

            if (full()) {
                head_ = (head_ + 1) % Capacity; // Advance head on overwrite
            } else {
                ++size_;
            }
        }

        // Remove and return the front element
        std::optional<T> pop()
        {
            if (empty()) {
                return std::nullopt;
            }

            T val = buffer_[head_];
            head_ = (head_ + 1) % Capacity;
            --size_;
            return val;
        }

        [[nodiscard]] bool empty() const { return size_ == 0; }
        [[nodiscard]] bool full() const { return size_ == Capacity; }
        [[nodiscard]] size_t size() const { return size_; }
        [[nodiscard]] size_t capacity() const { return Capacity; }
    };
} // namespace chai
