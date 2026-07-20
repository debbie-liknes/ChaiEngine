#include <Components/CameraComponent.h>
#include <Components/LightComponent.h>
#include <Components/MeshComponent.h>
#include <Components/TransformComponent.h>
#include <Scene/Scene.h>
#include <UI/Editor/PanelRegistry.h>
#include <UI/Editor/InternalChaiUI.h>
#include <EditorUI/SceneHierarchy.h>

namespace chai::scene
{
    Scene::Scene() {}

    ScenePanelIds Scene::registerPanels(ServiceLocator& locator)
    {
        std::string hierarchy = "Hierarchy";
        auto& panelRegistry = locator.resolve<ui::PanelRegistry>();
        panelRegistry.registerPanel({.id = hierarchy, .displayName = "Hierarchy", .draw = [&] {
                                         ui::drawSceneHierarchy(*this);
                                     }});

        return ScenePanelIds{hierarchy};
    }

    void Scene::update(const UpdateContext& ctx)
    {
        for (auto const& object : m_objects) {
            object->update(ctx);
        }
    }

    void Scene::extract(gfx::FrameRenderData& frame) const
    {
        for (auto const& object : m_objects) {
            object->extract(frame);
        }
    }

    GameObject* Scene::createObject(const std::string& name)
    {
        return m_objects
            .emplace_back(std::make_unique<GameObject>(name, gameObjAllocator_.allocate()))
            .get();
    }

    void Scene::setCamera(GameObject* cam)
    {
        camera_ = cam;
    }

    void Scene::setLight(GameObject* sun)
    {
        sun_ = sun;
    }
} // namespace chai::scene
