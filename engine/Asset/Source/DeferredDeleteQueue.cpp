#include <DeferredDeleteQueue.h>

namespace chai
{
    void DeferredDeleteQueue::beginFrame(std::uint64_t frameIndex) noexcept
    {
        currentFrame_ = frameIndex;
    }

    void DeferredDeleteQueue::enqueue(Deleter deleter)
    {
        pending_.emplace_back(currentFrame_, std::move(deleter));
    }

    void DeferredDeleteQueue::collect(std::uint64_t gpuRetiredFrame)
    {
        std::size_t w = 0;
        for (std::size_t r = 0; r < pending_.size(); r++) {
            if (pending_[r].frame <= gpuRetiredFrame) {
                pending_[r].deleter();
            } else {
                if (w != r)
                    pending_[w] = std::move(pending_[r]);
                w++;
            }
        }
        pending_.resize(w);
    }

    void DeferredDeleteQueue::flushAll()
    {
        for (auto const& e : pending_)
            e.deleter();
        pending_.clear();
    }

    std::size_t DeferredDeleteQueue::pendingCount() const noexcept
    {
        return pending_.size();
    }
}