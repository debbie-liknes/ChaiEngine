/**
 * @file DeferredDeleteQueue.h
 * @brief Holds destroy operations for GPU resources
 */

#pragma once

#include <cstdint>
#include <functional>
#include <utility>
#include <vector>

namespace chai
{
    /**
     * @brief Queue of delete operations for GPU resources that have been logically released but may
     * still be read by frames already submitted to the GPU.
     *
     * Deliberately type-erased (std::function) so a single graveyard serves
     */
    class DeferredDeleteQueue
    {
    public:
        using Deleter = std::function<void()>;

        /**
         * @brief Informs the queue of the current frame index
         *
         * @param frameIndex The index of the current frame, used to determine when pending
         * deletions can be safely executed
         */
        void beginFrame(std::uint64_t frameIndex) noexcept;

        /**
         * @brief Enqueues a delete operation
         *
         * @param Deleter The delete operation to be executed
         */
        void enqueue(Deleter deleter);

        /**
         * @brief Collects and executes delete operations that are safe to perform
         *
         * @param gpuRetiredFrame The index of the most recently retired frame on the GPU
         */
        void collect(std::uint64_t gpuRetiredFrame);

        /**
         * @brief Flushes all pending delete operations immediately
         */
        void flushAll();

        /**
         * @brief Returns the number of pending delete operations in the queue
         *
         * @return The number of pending delete operations
         */
        [[nodiscard]] std::size_t pendingCount() const noexcept;

    private:
        struct Entry {
            std::uint64_t frame;
            Deleter deleter;
        };

        std::uint64_t currentFrame_ = 0;
        std::vector<Entry> pending_;
    };

} // namespace chai