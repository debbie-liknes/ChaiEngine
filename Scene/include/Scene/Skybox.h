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

    private:
        void init();

        ResourceHandle m_cubeTextureHandle;
        MeshComponent* m_meshComponent;
    };
}