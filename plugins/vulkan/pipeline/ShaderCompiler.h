#pragma once
#include <Common/GraphicsEnums.h>
#include <filesystem>
#include <shaderc/shaderc.hpp>
#include <string>

namespace chai::gfx
{
    class ShaderCompiler
    {
    public:
        struct CompileResult {
            bool success = false;
            std::vector<uint32_t> spirv;
            std::string errorLog;
        };

        CompileResult compile(const std::filesystem::path& shaderPath, ShaderStage stage);

    private:
        std::string readFile(const std::filesystem::path& shaderPath);
    };
} // namespace chai::gfx