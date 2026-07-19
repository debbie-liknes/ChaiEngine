#include "SpdLogSink.h"
#include <LogLevel.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <vector>

namespace chai
{
    namespace
    {
        spdlog::level::level_enum toSpd(LogLevel level)
        {
            switch (level) {
                case LogLevel::Trace:
                    return spdlog::level::trace;
                case LogLevel::Debug:
                    return spdlog::level::debug;
                case LogLevel::Info:
                    return spdlog::level::info;
                case LogLevel::Warn:
                    return spdlog::level::warn;
                case LogLevel::Error:
                    return spdlog::level::err;
                case LogLevel::Critical:
                    return spdlog::level::critical;
                case LogLevel::Off:
                    return spdlog::level::off;
            }
            return spdlog::level::info;
        }

        std::shared_ptr<spdlog::logger> makeDefaultLogger()
        {
            auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            auto file = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                "logs/chai.log", 5 * 1024 * 1024, 3);

            std::vector<spdlog::sink_ptr> sinks{console, file};
            auto logger = std::make_shared<spdlog::logger>("chai", sinks.begin(), sinks.end());

            logger->set_level(spdlog::level::trace);
            logger->flush_on(spdlog::level::warn);
            logger->set_pattern("[%H:%M:%S.%e] [%^%l%$] [%s:%#] %v");
            return logger;
        }
    } // namespace

    SpdLogSink::SpdLogSink() : logger_(makeDefaultLogger()) {}

    SpdLogSink::SpdLogSink(std::shared_ptr<spdlog::logger> logger) : logger_(std::move(logger)) {}

    SpdLogSink::~SpdLogSink() = default;

    void SpdLogSink::write(const LogRecord& record)
    {
        logger_->log(spdlog::source_loc{record.file.data(), record.line, ""},
                     toSpd(record.level),
                     spdlog::string_view_t{record.message.data(), record.message.size()});
    }
} // namespace chai
