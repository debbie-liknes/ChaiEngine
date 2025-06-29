#pragma once
#include <ChaiGraphicsExport.h>
#include <string>
#include <variant>
#include <unordered_map>
#include <memory>
#include <ChaiEngine/UniformBuffer.h>

namespace chai::brew
{
    class UniformBufferBase;
    struct TextureSlot 
    {
        uint32_t id;
        int slot;
        std::string samplerName;
    };

    struct ShaderStage 
    {
        enum Type { Vertex, Fragment, Geometry, Compute };

        Type type;
        std::string sourceCode;
        std::string path;
        std::vector<std::string> defines;
        std::string entryPoint = "main";
    };

    struct ShaderDescription 
    {
        std::string name;
        std::vector<ShaderStage> stages;
        std::vector<std::string> globalDefines;

        // metadata
        struct 
        {
            bool isTransparent = false;
            bool requiresDepthTesting = true;
            bool writeToDepth = true;
        } renderHints;
    };

    class ShaderDescription;
    class IMaterial 
    {
    public:
        virtual ~IMaterial() = default;

        virtual void setTexture(const std::string& samplerName, uint32_t textureId, int slot = 0) = 0;
        virtual bool isTransparent() const = 0;
    };
}