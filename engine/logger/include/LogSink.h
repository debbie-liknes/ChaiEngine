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
     * @brief Interface for log sinks, which consume log records. This is to be implemented by
     * a separate log sink project. There is a separate spdLog prject. The reason for the 
     * separation is 1. Keep the possibility open to write another log sink, as it is now up to
     * the application to decide what sink to use, and 2. Not force everything that wants a log
     * to link to whatever logging library is being used.
     * 
     * The other library is NOT a plugin. We should be able to log things inside and outside
     * of plugin setup/teardown
     */
    class ILogSink
    {
    public:
        virtual ~ILogSink() = default;
        virtual void write(const LogRecord& record) = 0;
    };
} // namespace chai