#pragma once
#include <SceneExport.h>
#include <Scene/GameObject.h>

namespace chai::cup
{
    class SCENE_EXPORT Skybox : public GameObject
    {
    public:
        Skybox();


    private:
        void init();
        void setupShaders();

        ResourceHandle m_cubeTextureHandle;
        AssetHandle m_skyboxShaderAsset;
        MeshComponent* m_meshComponent;
    };
}