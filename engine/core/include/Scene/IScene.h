/**
 * @file IScene.h
 */
#pragma once
#include <Core/Updatable.h>
#include <cstdint>

namespace chai
{
    /**
     * @brief Interface for scene
     * @note I have my doubts that this should be here
     */
	class IScene
	{
    public:
        ~IScene() = default;

        virtual void update(const struct UpdateContext&) = 0;
        virtual void extract(gfx::FrameRenderData& frame) const = 0;
        virtual uint32_t getCameraId() const = 0;

	};
}