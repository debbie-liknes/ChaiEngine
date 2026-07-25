#include <Components/CameraComponent.h>
#include <Components/LightComponent.h>
#include <Components/MeshComponent.h>
#include <Components/TransformComponent.h>
#include <Scene/Scene.h>
#include <UI/Editor/PanelRegistry.h>
#include <UI/Editor/InternalChaiUI.h>
#include <EditorUI/SceneHierarchy.h>
#include <EditorUI/SceneObjectInspector.h>
#include <EditorUI/SceneUI.h>
#include <EditorUI/SelectionState.h>

namespace chai::scene
{
    Scene::Scene()
    {
        registerSceneIcons();
    }

    void setupEditorUI(ServiceLocator& locator)
    {
    }


    ScenePanelIds Scene::registerPanels(ServiceLocator& locator)
    {
        auto& panelRegistry = locator.resolve<ui::PanelRegistry>();
        auto& state = locator.resolve<ui::SelectionState>();

        std::string hierarchy = "Hierarchy";
        panelRegistry.registerPanel({.id = hierarchy, .displayName = "Hierarchy", .draw = [&] {
                                         ui::drawSceneHierarchy(*this);
                                     }});

        std::string inspector = "Inspector";
        panelRegistry.registerPanel({.id = inspector, .displayName = "Inspector", .draw = [&] {
                                         ui::drawInspectorPanel(*state.selected());
                                     }});

        return ScenePanelIds{hierarchy, inspector};
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
