/**
 * @file DecodePool.h
 */
#pragma once
#include <Loaders/ITextureLoader.h>
#include <mutex>
#include <thread>
#include <vector>
#include <condition_variable>

namespace chai::gfx
{
    /**
     * @brief Data that needs to be decoded via a loader (texture loader)
     */
    struct DecodeJob {
        AssetId id;
        std::vector<uint8_t> bytes;
        TextureFormat format;
    };

    /**
     * @brief results + id
     */
    struct DecodeResult {
        AssetId id;
        TextureAsset asset;
        bool jobSucceeded = false;
    };

    /**
     * @brief A Pool of worker threads to defer loading textures. Needed for the Model loading,
     * which have a buttload of textures
     * 
     * @note Probably need to make this more generic, but this is the only use I have for such a
     * thing at the moment. Will make this more generic when another need arises
     *
     */
    class DecodePool
    {
    public:
        explicit DecodePool(ITextureLoader& loader, uint32_t numThreads);
        virtual ~DecodePool();

        void enqueue(DecodeJob);
        std::vector<DecodeResult> collect();

    private:
        void workLoop();

        bool running_ = false;
        std::mutex jobMutex_, resultMutx_;
        std::condition_variable jobCv_;
        std::vector<std::thread> workers_;
        std::vector<DecodeJob> jobs_;
        std::vector<DecodeResult> results_;
        ITextureLoader& loader_;
    };
} // namespace chai::gfx
