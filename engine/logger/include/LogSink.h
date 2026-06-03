/**
 * @file LogSink.h
 */
#pragma once
#include <LogLevel.h>
#include <string_view>

namespace chai
{
    /**
     * @brief A log record, representing a single log event. This is the unit of data passed to log
     * sinks.
     */
    struct LogRecord {
        LogLevel level;
        std::string_view message;
        std::string_view file;
        int line;
    };

    /**
     * @brief Interface for log sinks, which consume log records. Implemented by the actual logging
     * backend (e.g. SpdlogSink).
     * 
     * @note write() must consume `message` synchronously
     */
    class ILogSink
    {
    public:
        virtual ~ILogSink() = default;
        virtual void write(const LogRecord& record) = 0;
    };
} // namespace chai