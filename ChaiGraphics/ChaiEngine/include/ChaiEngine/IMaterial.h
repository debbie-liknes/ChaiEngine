#pragma once
#include <ChaiGraphicsExport.h>
#include <string>
#include <variant>

namespace chai::brew
{
    class UniformBufferBase;
    struct TextureSlot {
        uint32_t textureId;
        int slot;
        std::string samplerName;
    };

    class IMaterial {
    public:
        virtual ~IMaterial() = default;

        // Core interface
        virtual void bind() const = 0;
        virtual void unbind() const = 0;

        // Uniform management
        virtual void setUniform(const std::string& name, const UniformBufferBase& value) = 0;
        virtual bool hasUniform(const std::string& name) const = 0;

        // Texture management
        virtual void setTexture(const std::string& samplerName, uint32_t textureId, int slot = 0) = 0;
        virtual void removeTexture(const std::string& samplerName) = 0;

        // Shader management
        virtual uint32_t getShaderId() const = 0;
        virtual bool isValid() const = 0;

        // State queries
        virtual bool isTransparent() const = 0;
        virtual int getRenderQueue() const = 0; // For sorting
    };
}