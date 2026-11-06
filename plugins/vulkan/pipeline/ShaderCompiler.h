/**
 * @file ShaderCompiler.h
 */
#pragma once
#include <Common/GraphicsEnums.h>
#include <filesystem>
#include <shaderc/shaderc.hpp>
#include <string>

namespace chai::gfx
{
    /**
     * @brief Runtime compiler for shaders. Produces spirv bytes
     */
    class ShaderCompiler
    {
        friend class ShaderIncluder;
    public:
        struct CompileResult {
            bool success = false;
            std::vector<uint32_t> spirv;
            std::string errorLog;
        };

        CompileResult compile(const std::filesystem::path& shaderPath, ShaderStage stage);

    private:
        static std::string readFile(const std::filesystem::path& shaderPath);
    };
} // namespace chai::gfx