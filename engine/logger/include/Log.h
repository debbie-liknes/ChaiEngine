/**
 * @file Log.h
 * @brief Logging interface for Bedrock and plugins.
 */
#pragma once
#include <LogLevel.h>
#include <LogSink.h>
#include <format>

namespace chai
{
    /**
     * @brief Set once at startup, the log sink is where all log records go
     */
    void addLogSink(ILogSink* sink);
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
// MSVC-specific: disable C26449 - gsl::span or std::string_view created from a temporary will be
//                                 invalid when the temporary is invalidated (gsl.view)
#define CHAI_LOG_AT(level, ...)                                                                    \
    do {                                                                                           \
        if constexpr ((level) >= CHAI_COMPILE_LOG_LEVEL) {                                         \
            _Pragma("warning(push)")                                                               \
            _Pragma("warning(disable : 26449)")                                                    \
            if (::chai::logEnabled(level)) {                                                       \
                ::chai::logRecord({(level), std::format(__VA_ARGS__), __FILE__, __LINE__});        \
            }                                                                                      \
            _Pragma("warning(pop)")                                                                \
        }                                                                                          \
    } while (false)

#define CHAI_LOG_TRACE(...) CHAI_LOG_AT(::chai::LogLevel::Trace, __VA_ARGS__)
#define CHAI_LOG_DEBUG(...) CHAI_LOG_AT(::chai::LogLevel::Debug, __VA_ARGS__)
#define CHAI_LOG_INFO(...) CHAI_LOG_AT(::chai::LogLevel::Info, __VA_ARGS__)
#define CHAI_LOG_WARN(...) CHAI_LOG_AT(::chai::LogLevel::Warn, __VA_ARGS__)
#define CHAI_LOG_ERROR(...) CHAI_LOG_AT(::chai::LogLevel::Error, __VA_ARGS__)
#define CHAI_LOG_CRITICAL(...) CHAI_LOG_AT(::chai::LogLevel::Critical, __VA_ARGS__)