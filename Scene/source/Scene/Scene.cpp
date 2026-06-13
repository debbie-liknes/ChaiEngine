#include <Scene/Scene.h>
#include <Components/LightComponent.h>
#include <Components/TransformComponent.h>
#include <Components/MeshComponent.h>
#include <Components/CameraComponent.h>

namespace chai::scene
{
    void Scene::update(float deltaTime)
    {
        for (auto const& object : m_objects) {
            object->update(deltaTime);
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
        return m_objects.emplace_back(std::make_unique<GameObject>(name)).get();
    }

    void Scene::setCamera(GameObject* cam)
    {
        camera_ = cam;
    }

    void Scene::setLight(GameObject* sun)
    {
        sun_ = sun;
    }

    void Scene::setCameraAspect(float aspect)
    {
        camera_->getComponent<CameraComponent>()->setAspectRatio(aspect);
    }
}