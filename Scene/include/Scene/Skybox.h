#pragma once
#include <SceneExport.h>
#include <Scene/GameObject.h>

namespace chai::cup
{
    class SCENE_EXPORT Skybox : public GameObject
    {
    public:
        Skybox();

        void collectRenderables(brew::RenderCommandCollector& collector) override;
        bool isVisible(const Frustum& frustum) override { return true; }

    private:
        void init();

        ResourceHandle m_cubeTextureHandle;
        PipelineState m_skyboxPipelineState;
    };
}