#include <Loader/GLSLLoader.h>
#include <filesystem>
#include <fstream>
#include <Graphics/ShaderAsset.h>

#include <regex>

namespace chai
{
    bool GLSLLoader::canLoad(const std::string& ext) const
    {
        return ext == "vert" || ext == "frag";
    }

    std::unique_ptr<IAsset> GLSLLoader::load(const std::string& path)
    {
        auto shaderAsset = std::make_unique<ShaderStageAsset>();
        ShaderStageSource shaderSource;

        std::filesystem::path filePath(path);
        if (filePath.extension() == ".vert")
        {
            shaderSource.stage = ShaderStage::Vertex;
        }
        else if (filePath.extension() == ".frag")
        {
            shaderSource.stage = ShaderStage::Fragment;
        }

        std::ifstream shaderFile(path);
        if (!shaderFile.is_open())
        {
            throw std::runtime_error("Can't open file " + path + ".");
        }

        std::stringstream shaderSourceStream;
        shaderSourceStream << shaderFile.rdbuf();

        shaderFile.close();

        shaderSource.sourceCode = shaderSourceStream.str();
        shaderAsset->source = shaderSource;

        return std::move(shaderAsset);
    }
}

CHAI_PLUGIN_SERVICES(ShaderLoaderPlugin)
{
    CHAI_LOADER(chai::GLSLLoader, "GLSL loader")
}

CHAI_DEFINE_PLUGIN_ENTRY(ShaderLoaderPlugin, "GLSLLoader", "1.0.0", GLSLLOADER_EXPORT)