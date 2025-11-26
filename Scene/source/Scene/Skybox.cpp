#include <ChaiEngine/MaterialSystem.h>
#include <Components/MeshComponent.h>
#include <Components/TransformComponent.h>
#include <Graphics/TextureAsset.h>

#include <Scene/Skybox.h>

namespace chai::cup
{
    Skybox::Skybox() : m_meshComponent(nullptr)
    {
        setupShaders();
        init();
    }

    void Skybox::init()
    {
        m_meshComponent = addComponent<chai::cup::MeshComponent>();
        auto meshAsset = chai::AssetManager::instance().load<chai::MeshAsset>("assets/skybox.obj");
        m_meshComponent->setMesh(meshAsset.value());
        m_cubeTextureHandle = loadTextureCube("assets/skybox").value();

        auto material = chai::MaterialSystem::Builder(m_skyboxShaderAsset)
        .setTexture("u_Skybox", m_cubeTextureHandle)
        .build();

        auto materialInstance = std::make_unique<chai::MaterialInstance>(material);

        //set pipeline state
        m_meshComponent->getPipelineState().rasterState.cullMode = RasterizerState::CullMode::None;
        m_meshComponent->getPipelineState().depthStencilState.depthCompareOp = DepthStencilState::CompareOp::LessEqual;

        // Customize instance
        m_meshComponent->setMaterial(
            chai::ResourceManager::instance().add<chai::MaterialInstance>(std::move(materialInstance)));
    }

    void Skybox::setupShaders()
    {
        auto vertAssetHandle = AssetManager::instance().load<ShaderStageAsset>("shaders/skybox.vert").value();
        auto fragAssetHandle = AssetManager::instance().load<ShaderStageAsset>("shaders/skybox.frag").value();

        // Get the loaded stage assets
        auto vertAsset = AssetManager::instance().get<ShaderStageAsset>(vertAssetHandle);
        auto fragAsset = AssetManager::instance().get<ShaderStageAsset>(fragAssetHandle);

        auto shaderAsset = std::make_unique<ShaderAsset>("skybox_shader");
        shaderAsset->addStage(*vertAsset);
        shaderAsset->addStage(*fragAsset);

        shaderAsset->addVertexInput("a_Position", 0, DataType::Float3);

        shaderAsset->addUniform("u_Skybox", DataType::SamplerCube);

        m_skyboxShaderAsset = AssetManager::instance().add(std::move(shaderAsset)).value();
    }
}