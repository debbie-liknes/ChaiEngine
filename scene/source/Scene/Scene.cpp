#include <Components/CameraComponent.h>
#include <Components/LightComponent.h>
#include <Components/MeshComponent.h>
#include <Components/TransformComponent.h>
#include <Scene/Scene.h>
#include <UI/Editor/InternalChaiUI.h>
#include <Visitors/FrameRenderVisitor.h>
#include <tracy/Tracy.hpp>

namespace chai::scene
{
    Scene::Scene() {}

    void Scene::update(const UpdateContext& ctx)
    {
        ZoneScoped

        for (auto const& object : m_objects) {
            object->update(ctx);
        }
    }

    void Scene::accept(Visitor* visitor)
    {
        for (auto const& object : m_objects) {
            object->accept(visitor);
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
