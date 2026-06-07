/**
 * @file Log.h
 * @brief Logging interface for Bedrock and plugins.
 */
#pragma once
#include <LogSink.h>
#include <LogLevel.h>
#include <format>

namespace chai
{
    /**
     * @brief Set once at startup, the log sink is where all log records go
     */
    void setLogSink(ILogSink* sink);
    void setLogLevel(LogLevel level);
    LogLevel getLogLevel();

    inline bool logEnabled(LogLevel level)
    {
        return level >= getLogLevel();
    }

    /**
     * @brief Callable directly by the CHAI_LOG macros, not intended for direct use. The record is
     * already formatted and includes source location.
     */
    void logRecord(const LogRecord& record);
} // namespace chai

#ifndef CHAI_COMPILE_LOG_LEVEL
#define CHAI_COMPILE_LOG_LEVEL ::chai::LogLevel::Trace
#endif

/**
 * @brief Logging macros. These will check the compile-time log level first, then the runtime log
 * level
 */
#define CHAI_LOG_AT(level, ...)                                                                    \
    if constexpr ((level) >= CHAI_COMPILE_LOG_LEVEL) {                                             \
        if (::chai::logEnabled(level)) {                                                           \
            ::chai::logRecord(                                                                     \
                ::chai::LogRecord{(level), ::std::format(__VA_ARGS__), __FILE__, __LINE__});       \
        }                                                                                          \
    }  

#define CHAI_LOG_TRACE(...) CHAI_LOG_AT(::chai::LogLevel::Trace, __VA_ARGS__)
#define CHAI_LOG_DEBUG(...) CHAI_LOG_AT(::chai::LogLevel::Debug, __VA_ARGS__)
#define CHAI_LOG_INFO(...) CHAI_LOG_AT(::chai::LogLevel::Info, __VA_ARGS__)
#define CHAI_LOG_WARN(...) CHAI_LOG_AT(::chai::LogLevel::Warn, __VA_ARGS__)
#define CHAI_LOG_ERROR(...) CHAI_LOG_AT(::chai::LogLevel::Error, __VA_ARGS__)
#define CHAI_LOG_CRITICAL(...) CHAI_LOG_AT(::chai::LogLevel::Critical, __VA_ARGS__)