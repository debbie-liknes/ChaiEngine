#include "ShaderCompiler.h"

#include <fstream>
#include <sstream>
#include "ShaderIncluder.h"

namespace chai::gfx
{
    static shaderc_shader_kind toShadercKind(ShaderStage stage)
    {
        switch (stage) {
            case ShaderStage::Vertex:
                return shaderc_glsl_vertex_shader;
            case ShaderStage::Fragment:
                return shaderc_glsl_fragment_shader;
            default:
                throw std::runtime_error("Unsupported shader stage");
        }
    }

    ShaderCompiler::CompileResult ShaderCompiler::compile(const std::filesystem::path& shaderPath,
                                                          ShaderStage stage)
    {
        std::string source = readFile(shaderPath);
        if (source.empty()) {
            return {false, {}, "Failed to read file: " + shaderPath.string()};
        }

        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
        options.SetOptimizationLevel(shaderc_optimization_level_performance);

        //NOTE: Having this created for every compile allows us to not have to reset includer caching
        options.SetIncluder(std::make_unique<ShaderIncluder>());

        shaderc_shader_kind kind = toShadercKind(stage);

        //auto result1 =
        //    compiler.PreprocessGlsl(source, shaderc_glsl_fragment_shader, "pbr.frag", options);
        //std::string expanded(result1.cbegin(), result1.cend());

        shaderc::SpvCompilationResult result =
            compiler.CompileGlslToSpv(source, kind, shaderPath.string().c_str(), options);

        if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
            return {false, {}, result.GetErrorMessage()};
        }

        return {true, {result.cbegin(), result.cend()}, ""};
    }

    std::string ShaderCompiler::readFile(const std::filesystem::path& path)
    {
        std::ifstream file(path);
        if (!file.is_open())
            return "";
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }
} // namespace chai::gfx