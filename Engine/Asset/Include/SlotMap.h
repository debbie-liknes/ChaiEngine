/**
 * @file SlotMap.h
 * @brief Maps Handle<T> to V with O(1) insert/get/erase/valid
 */
#pragma once

#include "Handle.h"

#include <cstdint>
#include <type_traits>
#include <utility>
#include <vector>

namespace chai
{
    /**
     * @brief An array addressed by Handle<T> that owns values of type V
     * @note V must be default-constructible and move-assignable
     * @return True if the handle is valid, false if it's invalid
     */
    template <typename T, typename V>
    class SlotMap
    {
        static_assert(std::is_default_constructible_v<V>,
                      "SlotMap<V>: V must be default-constructible");
        static_assert(std::is_move_assignable_v<V>, "SlotMap<V>: V must be move-assignable");

    public:
        using HandleType = Handle<T>;

        /**
         * @brief Insert a value into the slot map
         * @param value The value to insert
         * @return Handle to the inserted value.
         */
        [[nodiscard]] HandleType insert(V value);

        /**
         * @brief Construct a value in-place in the slot map
         * @param args Arguments to forward to V's constructor
         * @return Handle to the inserted value
         */
        template <typename... Args>
        [[nodiscard]] HandleType emplace(Args&&... args);

        /**
         * @brief Get a pointer to the value associated with a handle
         * @param h The handle to look up
         * @return Pointer to the value if the handle is valid, nullptr otherwise
         */
        [[nodiscard]] V* get(HandleType h) noexcept;

        /**
         * @brief Get a pointer to the value associated with a handle
         * @param h The handle to look up
         * @return Const pointer to the value if the handle is valid, nullptr otherwise
         */
        [[nodiscard]] const V* get(HandleType h) const noexcept;

        /**
         * @brief Check whether a handle is valid
         * @param h The handle to look up
         * @return The validity of the handle
         */
        [[nodiscard]] bool valid(HandleType h) const noexcept;

        /**
         * @brief Erase the value associated with a handle
         * @param h The handle to erase
         * @return True if the handle was valid and the value was erased, false if the handle was
         * invalid
         */
        bool erase(HandleType h);

        /**
         * @brief The number of live values in the slot map
         */
        [[nodiscard]] std::size_t size() const noexcept { return liveCount_; }

        /**
         * @brief The number of total slots in the slot map, including free ones
         */
        [[nodiscard]] std::size_t capacity() const noexcept { return slots_.size(); }

        /**
         * @brief Empties the slot map, invalidating all handles
         */
        void clear();

    private:
        struct Slot {
            std::uint32_t generation = 0;
            bool occupied = false;
        };

        std::vector<Slot> slots_; // parallel to values_
        std::vector<V> values_;   // parallel to slots_
        std::vector<std::uint32_t> freeList_;
        std::size_t liveCount_ = 0;
    };

    //////////////////////////////// implementation below /////////////////////////////////
    template <typename T, typename V>
    typename SlotMap<T, V>::HandleType SlotMap<T, V>::insert(V value)
    {
        return emplace(std::move(value));
    }

    template <typename T, typename V>
    template <typename... Args>
    typename SlotMap<T, V>::HandleType SlotMap<T, V>::emplace(Args&&... args)
    {
        std::uint32_t idx;
        if (!freeList_.empty()) {
            // reuse a free slot
            idx = freeList_.back();
            freeList_.pop_back();
            values_[idx] = V(std::forward<Args>(args)...);
            slots_[idx].occupied = true;
        } else {
            // No free slot: add a new one at the end
            idx = static_cast<std::uint32_t>(slots_.size());
            slots_.push_back(Slot{0, true});
            values_.emplace_back(std::forward<Args>(args)...);
        }
        liveCount_++;
        return HandleType{idx, slots_[idx].generation};
    }

    template <typename T, typename V>
    bool SlotMap<T, V>::valid(HandleType h) const noexcept
    {
        return h.valid() && h.index < slots_.size() && slots_[h.index].occupied &&
               slots_[h.index].generation == h.generation;
    }

    template <typename T, typename V>
    V* SlotMap<T, V>::get(HandleType h) noexcept
    {
        return valid(h) ? &values_[h.index] : nullptr;
    }

    template <typename T, typename V>
    const V* SlotMap<T, V>::get(HandleType h) const noexcept
    {
        return valid(h) ? &values_[h.index] : nullptr;
    }

    template <typename T, typename V>
    bool SlotMap<T, V>::erase(HandleType h)
    {
        if (!valid(h))
            return false;

        // Invalidate the slot and release the value
        Slot& s = slots_[h.index];
        s.occupied = false;
        s.generation++;
        values_[h.index] = V{}; //release the value
        freeList_.push_back(h.index);
        --liveCount_;
        return true;
    }

    template <typename T, typename V>
    void SlotMap<T, V>::clear()
    {
        slots_.clear();
        values_.clear();
        freeList_.clear();
        liveCount_ = 0;
    }
} // namespace chai