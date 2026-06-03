/**
 * @file Clock.h
 * @brief Simple clock class that tracks elapsed time between ticks
 */
#pragma once
#include <chrono>

namespace chai
{
    /**
     * @brief Thin wrapper around std::chrono
     */
    class Clock
    {
    public:
        float tick()
        {
            auto now = std::chrono::steady_clock::now();
            std::chrono::duration<float> delta = now - last_;
            last_ = now;
            return delta.count();
        }

    private:
        std::chrono::steady_clock::time_point last_ = std::chrono::steady_clock::now();
    };
}