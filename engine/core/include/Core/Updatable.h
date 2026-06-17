/**
 * @file Updatable.h
 */
#pragma once
#include <CoreExport.h>

namespace chai
{
	namespace gfx
	{
        struct FrameRenderData;
	}

	struct UpdateContext;

	/**
     * @brief Interface for anything that needs to be updated every frame
	 * @todo Figure out if this is needed in the Core or the Scene, decide
	 * if this should be moved to 2 different interfaces
     */
	class CORE_EXPORT IUpdatable
	{
    public:
        virtual ~IUpdatable() = default;

		virtual void update(const UpdateContext&) {}
        virtual void extract(gfx::FrameRenderData& frame) const {}
	};
}