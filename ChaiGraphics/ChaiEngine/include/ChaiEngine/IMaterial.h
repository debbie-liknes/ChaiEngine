#pragma once
#include <ChaiGraphicsExport.h>
#include <string>

namespace chai::brew
{
    class IMaterial {
    public:
        virtual ~IMaterial() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        // Uniform setters (common interface for all APIs)
        virtual void SetMatrix4(const std::string& name, const float* matrix) = 0;
        virtual void SetVector3(const std::string& name, float x, float y, float z) = 0;
        virtual void SetVector4(const std::string& name, float x, float y, float z, float w) = 0;
        virtual void SetFloat(const std::string& name, float value) = 0;
        virtual void SetInt(const std::string& name, int value) = 0;
        virtual void SetTexture(const std::string& name, unsigned int textureSlot) = 0;

        //batching?
        virtual void BeginBatch() {}
        virtual void EndBatch() {}
    };
}