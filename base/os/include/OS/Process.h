#pragma once

#include <string>
#include <vector>

namespace chai
{
    class Process
    {
    public:
        struct Info
        {
            std::string path;
            std::vector<std::string> args;
            std::vector<std::string> env;
        };

        explicit Process(const Info& info) : info_(info) {}

        virtual ~Process() = default;

        /**
         * @brief Executes process, performing a blocking wait until completion.
         * @return exit code of process
         */
        virtual int execute() = 0;

    protected:
        Info info_;
    };
} // namespace chai

