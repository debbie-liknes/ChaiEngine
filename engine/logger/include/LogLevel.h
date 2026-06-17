/**
 * @file LogLevel.h
 * @brief Defines the LogLevel enum and related utilities for the CHAI logging system.
 */
#pragma once
#include <string_view>

namespace chai
{
    /**
     * @brief Ordered from least to most severe
     */
    enum class LogLevel { Trace = 0, Debug, Info, Warn, Error, Critical, Off };

    constexpr std::string_view toString(LogLevel level)
    {
        switch (level) {
            case LogLevel::Trace:
                return "TRACE";
            case LogLevel::Debug:
                return "DEBUG";
            case LogLevel::Info:
                return "INFO";
            case LogLevel::Warn:
                return "WARN";
            case LogLevel::Error:
                return "ERROR";
            case LogLevel::Critical:
                return "CRIT";
            case LogLevel::Off:
                return "OFF";
        }
        return "?";
    }
} // namespace chai