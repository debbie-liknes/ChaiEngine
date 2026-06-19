#include <Log.h>
#include <atomic>

namespace chai
{
    namespace
    {
        std::atomic<ILogSink*> g_sink{nullptr};
        std::atomic<LogLevel> g_level{LogLevel::Info};
    } // namespace

    void setLogSink(ILogSink* sink)
    {
        g_sink.store(sink, std::memory_order_release);
    }

    void setLogLevel(LogLevel level)
    {
        g_level.store(level, std::memory_order_relaxed);
    }

    LogLevel getLogLevel()
    {
        return g_level.load(std::memory_order_relaxed);
    }

    void logRecord(const LogRecord& record)
    {
        if (auto* sink = g_sink.load(std::memory_order_acquire))
            sink->write(record);
        // No sink yet, just drop it
    }
} // namespace chai