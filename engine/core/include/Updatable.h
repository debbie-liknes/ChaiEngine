#pragma once
#include <CoreExport.h>

namespace chai
{
	namespace gfx
	{
        struct FrameRenderData;
	}

	class CORE_EXPORT IUpdatable
	{
    public:
        virtual ~IUpdatable() = default;

		virtual void update(float deltaTime) {}
        virtual void extract(gfx::FrameRenderData& frame) const {}
	};
}