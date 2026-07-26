#pragma once

#include <Visitors/Visitor.h>
#include <Rendering/FrameRenderData.h>

namespace chai::scene
{
    class FrameRenderVisitor : public Visitor
    {
    public:
        virtual void visit(GameObject* node) override;
        virtual void visit(Component* node) override;

        virtual void reset() override
        {
            renderData_ = gfx::FrameRenderData();
        }

        [[nodiscard]] gfx::FrameRenderData& getData()
        {
            return renderData_;
        }

    private:
        gfx::FrameRenderData renderData_;
    };
}
