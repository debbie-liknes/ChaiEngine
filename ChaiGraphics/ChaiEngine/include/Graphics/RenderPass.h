#pragma once
#include <ChaiMath.h>
#include <vector>
#include <Graphics/RenderKey.h>

namespace chai::brew
{
    struct RenderPassDesc 
    {
        enum class Type { GBuffer, Lighting, Forward, Shadow, PostProcess, Skybox };

        Type type;
        uint32_t framebuffer = 0; // 0 = default framebuffer
        bool clearColor = true;
        bool clearDepth = true;
        Vec4 clearColorValue = Vec4(0, 0, 0, 1);
        float clearDepthValue = 1.0f;
    };

    class RenderPass
    {
    public:
        virtual ~RenderPass() = default;
        virtual void setup(void* backend) = 0;
        virtual void execute(void* backend,
                             const std::vector<SortedDrawCommand>& draws) = 0;
        virtual void resize(int width, int height) {}

        const RenderPassDesc& getDesc() const { return m_desc; }

    protected:
        RenderPassDesc m_desc;
    };
}