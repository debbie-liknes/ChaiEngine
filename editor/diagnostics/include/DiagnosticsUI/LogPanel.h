#pragma once
#include <LogSink.h>

#include <deque>
#include <mutex>
#include <string>
#include <vector>

namespace chai::diagnostics
{
    //Register this
    class GuiLogSink : public ILogSink
    {
    public:
        struct Entry {
            LogLevel level;
            std::string message;
        };

        void write(const LogRecord& record) override;

        void getEntries(std::vector<Entry>& out) const;
        void clear();

    private:
        static constexpr size_t kMaxEntries = 2000;
        mutable std::mutex mutex_;
        std::deque<Entry> entries_;
    };

    void drawLogPanel(GuiLogSink& sink);
}