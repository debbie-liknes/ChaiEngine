#include <ChaiEngine/MaterialSystem.h>
#include <Components/MeshComponent.h>
#include <Components/TransformComponent.h>
#include <Graphics/TextureAsset.h>

#include <Scene/Skybox.h>

namespace chai::cup
{
    Skybox::Skybox()
    {
        init();
    }

    void Skybox::init()
    {
        /*m_meshComponent = addComponent<chai::cup::MeshComponent>();
        auto meshAsset = chai::AssetManager::instance().load<chai::MeshAsset>("assets/skybox.obj");
        m_meshComponent->setMesh(meshAsset.value());*/
        m_cubeTextureHandle = loadTextureCube("assets/skybox").value();

        /*auto material = chai::MaterialSystem::Builder(m_skyboxShaderAsset)
        .setTexture("u_Skybox", m_cubeTextureHandle)
        .build();

        auto materialInstance = std::make_unique<chai::MaterialInstance>(material);

        //set pipeline state
        m_meshComponent->getPipelineState().rasterState.cullMode = RasterizerState::CullMode::None;
        m_meshComponent->getPipelineState().depthStencilState.depthCompareOp = DepthStencilState::CompareOp::LessEqual;

        // Customize instance
        m_meshComponent->setMaterialInstance(
            chai::ResourceManager::instance().add<chai::MaterialInstance>(std::move(materialInstance)));*/

        m_skyboxPipelineState.rasterState.cullMode = RasterizerState::CullMode::None;
        m_skyboxPipelineState.depthStencilState.depthCompareOp = DepthStencilState::CompareOp::LessEqual;
        m_skyboxPipelineState.depthStencilState.depthTestEnable = true;
        //m_skyboxPipelineState.depthStencilState.mas
    }

    void Skybox::collectRenderables(brew::RenderCommandCollector& collector)
    {
        if (!m_cubeTextureHandle.isValid()) return;

        brew::RenderCommand cmd;
        cmd.type = brew::RenderCommand::DRAW_SKYBOX;
        cmd.skybox = m_cubeTextureHandle;
        cmd.pipelineState = m_skyboxPipelineState;
        collector.submit(cmd);
    }
}