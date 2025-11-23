#pragma once
#include <ChaiGraphicsExport.h>
#include <string>
#include <memory>
#include <ChaiEngine/UniformBuffer.h>

namespace chai
{
class CHAIGRAPHICS_EXPORT IMaterial
{
public:
    virtual ~IMaterial() = default;

    virtual void setTexture(const std::string& samplerName, uint32_t textureId, int slot = 0) = 0;
    virtual bool isTransparent() const = 0;
};
}