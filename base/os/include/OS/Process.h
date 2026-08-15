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

        virtual ~Process() = default;

        /**
         * @brief Executes process, performing a blocking wait until completion.
         * @return exit code of process
         */
        virtual int execute() = 0;

    protected:
        // Explicitly disallow construction of the base class
        // May be constructed via platform libraries
        Process() = default;
    };
} // namespace chai

