#pragma once
#include <CoreExport.h>

namespace chai
{
	namespace gfx
	{
        struct FrameRenderData;
	}

	struct UpdateContext;

	class CORE_EXPORT IUpdatable
	{
    public:
        virtual ~IUpdatable() = default;

		virtual void update(const UpdateContext&) {}
        virtual void extract(gfx::FrameRenderData& frame) const {}
	};
}