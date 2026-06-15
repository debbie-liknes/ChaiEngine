#pragma once
#include <Updatable.h>

namespace chai
{
	class IScene
	{
    public:
        ~IScene() = default;

        virtual void update(const struct UpdateContext&) = 0;
        virtual void extract(gfx::FrameRenderData& frame) const = 0;
        virtual void setCameraAspect(float aspect) = 0;
	};
}