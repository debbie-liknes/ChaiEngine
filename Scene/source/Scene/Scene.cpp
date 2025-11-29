#include <Scene/Scene.h>
#include <Components/LightComponent.h>
#include <Components/TransformComponent.h>
#include <Components/MeshComponent.h>

namespace chai::cup
{
    void Scene::addGameObject(std::unique_ptr<GameObject> object)
    {
        m_objects.push_back(std::move(object));
    }

    void Scene::collectRenderables(brew::RenderCommandCollector& collector) const
    {
        //do this in a scene graph traversal?
        for (auto& object : m_objects)
        {
            object->collectRenderables(collector);
        }
    }

    void Scene::collectLights(brew::RenderCommandCollector& collector) const
    {
        brew::RenderCommand cmd;
        cmd.type = brew::RenderCommand::SET_LIGHTS;
        for (auto& object : m_objects)
        {
            auto lightComp = object->getComponent<LightComponent>();
            if (!lightComp || !lightComp->enabled)
                continue;

            //we should limit the number of lights to a reasonable amount
            //should we cache this? lights dont change much

            auto transform = object->getComponent<TransformComponent>();

            brew::Light light = {};
            light.positionAndType = Vec4(transform->getWorldPosition(),
                                         static_cast<int>(lightComp->type));
            light.color = Vec4(lightComp->color, lightComp->intensity);
            light.directionAndRange = Vec4(transform->forward(), lightComp->range);
            light.spotParams = Vec4(lightComp->innerCone, lightComp->outerCone, 0.0, 0.0);

            cmd.lights.push_back(light);
        }
        if (cmd.lights.empty())
            return;

        collector.submit(cmd);
    }

    void Scene::update(double deltaTime)
    {
        for (auto const& object : m_objects)
        {
            object->update(deltaTime);
        }
    }

    GameObject* Scene::createGameObject(const std::string& name) 
    {
        return m_objects.emplace_back(std::make_unique<GameObject>(name)).get();
    }

    GameObject* Scene::createModelObject(const std::string& name,
                                                         AssetHandle modelHandle)
    {
        auto* model = AssetManager::instance().get<ModelAsset>(modelHandle);
        if (!model)
            return nullptr;

        // Create root object for the whole model
        auto root = createGameObject(name);

        std::vector<GameObject*> nodeObjects;

        // Create GameObjects for each node
        for (auto& node : model->nodes) {
            auto go = createGameObject(node.name);
            go->getComponent<TransformComponent>()->setLocalMatrix(node.localTransform);

            if (node.meshIndex >= 0) {
                auto* meshComp = go->addComponent<MeshComponent>();
                meshComp->setMesh(model->meshes[node.meshIndex]);
            }

            nodeObjects.push_back(go);
        }

        // Setup hierarchy from model
        for (size_t i = 0; i < model->nodes.size(); i++) {
            if (model->nodes[i].parentIndex >= 0) {

                nodeObjects[i]->setParent(nodeObjects[model->nodes[i].parentIndex]);
            } else {
                // Root nodes in the model become children of our root object
                nodeObjects[i]->setParent(root);
            }
        }

        return root;
    }
}