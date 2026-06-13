#pragma once
#include <Updatable.h>

namespace chai
{
	class IScene
	{
    public:
        ~IScene() = default;

        virtual void update(float dt) = 0;
        virtual void extract(gfx::FrameRenderData& frame) const = 0;
        virtual void setCameraAspect(float aspect) = 0;
	};
}