/**
 * @file Updatable.h
 */
#pragma once

namespace chai
{
	namespace gfx
	{
        class FrameRenderVisitor;
	}

	namespace audio
	{
        struct AudioSceneData;
	}

	struct UpdateContext;

	/**
     * @brief Interface for anything that needs to be updated every frame
	 * @todo Figure out if this is needed in the Core or the Scene, decide
	 * if this should be moved to 2 different interfaces
     */
	class IUpdatable
	{
    public:
        virtual ~IUpdatable() = default;

		virtual void update(const UpdateContext&) {}
        virtual void accept(gfx::FrameRenderVisitor& visitor) const {}
	};
}