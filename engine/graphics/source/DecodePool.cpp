#include <Assets/DecodePool.h>

namespace chai::gfx
{
    DecodePool::DecodePool(ITextureLoader& loader, uint32_t numThreads) : loader_(loader)
    {
        running_ = true;

        for (unsigned i = 0; i < numThreads; ++i)
            workers_.emplace_back([this] { workLoop(); });
    }

    DecodePool::~DecodePool()
    {
        {
            std::unique_lock lk(jobMutex_);
            running_ = false;
        }
        jobCv_.notify_all();
        for (auto& t : workers_)
            t.join();
    }

    void DecodePool::workLoop()
    {
        while (true) {
            DecodeJob job;
            {
                std::unique_lock lk(jobMutex_);
                jobCv_.wait(lk, [this] { return !running_ || !jobs_.empty(); }); //tell the thread to sleep if there are no jobs
                if (!running_ && jobs_.empty())
                    return;
                job = jobs_.front();
                jobs_.erase(jobs_.begin());
            }

            DecodeResult result{job.id};
            auto asset = loader_.decode(job.bytes, job.format);

            if (asset) {
                result.asset = asset.value();
                result.jobSucceeded = true;
            }

            {
                std::unique_lock lk(resultMutx_);
                results_.push_back(result);
            }
        }
    }

    void DecodePool::enqueue(DecodeJob job)
    {
        std::unique_lock lk(jobMutex_);
        jobs_.push_back(std::move(job));
        jobCv_.notify_one();    //tell one worker thread to wake up
    }

    std::vector<DecodeResult> DecodePool::collect()
    {
        std::vector<DecodeResult> res;
        {
            std::unique_lock lk(resultMutx_);
            for (auto& r : results_) {
                res.push_back(std::move(r));
            }
            results_.clear();
        }

        return res;
    }
}