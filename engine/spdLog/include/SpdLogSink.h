/**
 * @file SpdLogSink.h
 * @brief An ILogSink implementation backed by spdlog
 */
#pragma once
#include <LogSink.h>
#include <memory>

//Please note there are NO SPDLOG INCLUDES HERE
//Dont put any in this header pls
namespace spdlog
{
    class logger;
}

namespace chai
{
    /**
     * @brief An ILogSink implementation backed by spdlog
     */
    class SpdlogSink : public ILogSink
    {
    public:
        SpdlogSink();

        // Probably dont need this, but just in case someone wants to use an existing spdlog logger
        explicit SpdlogSink(std::shared_ptr<spdlog::logger> logger);

        ~SpdlogSink() override;

        /**
         * @brief Write a log record to the sink. This will be called by the CHAI_LOG macros after
         * formatting the message.
         */
        void write(const LogRecord& record) override;

    private:
        std::shared_ptr<spdlog::logger> logger_;
    };
} // namespace chai