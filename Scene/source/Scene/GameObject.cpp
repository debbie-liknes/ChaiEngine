#include <Components/MeshComponent.h>
#include <Components/TransformComponent.h>
#include <Core/Updatable.h>
#include <Scene/GameObject.h>

namespace chai::cup
{
    GameObject::GameObject()
    {
        addComponent<TransformComponent>(this);
    }

    GameObject::GameObject(const std::string& name) : m_name(name)
    {
        addComponent<TransformComponent>(this);
    }

    void GameObject::setParent(GameObject* parent)
    {
        m_parent = parent;
    }

    GameObject* GameObject::getParent() const
    {
        return m_parent;
    }

    const std::vector<GameObject*>& GameObject::getChildren() const
    {
        return m_children;
    }

    void GameObject::collectRenderables(brew::RenderCommandCollector& collector)
    {
        for (const auto& component : m_components) {
            if (auto renderable = dynamic_cast<MeshComponent*>(component.get())) {
                auto* meshResource =
                    ResourceManager::instance().getResource<MeshResource>(renderable->getMesh());
                if (!meshResource)
                    continue;

                Mat4 worldTransform = getComponent<TransformComponent>()->getWorldMatrix();

                for (size_t i = 0; i < meshResource->submeshes.size(); i++) {
                    const auto& submesh = meshResource->submeshes[i];

                    brew::RenderCommand cmd;
                    cmd.type = brew::RenderCommand::DRAW_MESH;
                    cmd.mesh = renderable->getMesh();
                    cmd.indexOffset = submesh.indexOffset;
                    cmd.indexCount = submesh.indexCount;
                    cmd.transform = worldTransform;
                    cmd.pipelineState = renderable->getPipelineState();

                    // Get material - check override first, then submesh default
                    ResourceHandle mat = renderable->getMaterial(i);
                    if (!mat.isValid() && submesh.material.isValid()) {
                        // submesh.material is an AssetHandle, need to get/create resource
                        //mat = getOrCreateMaterialResource(submesh.material);
                    }
                    cmd.material = mat;

                    collector.submit(cmd);
                }
            }
        }
    }

    void GameObject::update(double deltaTime)
    {
        for (const auto& component : m_components) {
            if (auto updatable = dynamic_cast<IUpdatable*>(component.get())) {
                updatable->update(deltaTime);
            }
        }

        if (controllerComponent) {
            controllerComponent->update(deltaTime);
        }
    }
} // namespace chai::cup