#include <Log.h>
#include <atomic>
#include <mutex>
#include <vector>

namespace chai
{
    namespace
    {
        std::mutex g_sinkMutex;
        std::vector<ILogSink*> g_sinks;
        std::atomic<LogLevel> g_level{LogLevel::Info};
    } // namespace

    void addLogSink(ILogSink* sink)
    {
        std::lock_guard lock(g_sinkMutex);
        g_sinks.push_back(sink);
    }

    void removeLogSink(ILogSink* sink)
    {
        std::lock_guard lock(g_sinkMutex);
        std::erase(g_sinks, sink);
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
        std::lock_guard lock(g_sinkMutex);
        for (auto* sink : g_sinks)
            sink->write(record);
    }
} // namespace chai