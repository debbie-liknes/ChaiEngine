#pragma once
#include <vector>
#include <ChaiEngine/Light.h>
#include <Scene/Camera.h>
#include <Scene/GameObject.h>
#include <ChaiEngine/RenderCommandCollector.h>
#include <Core/Updatable.h>

namespace chai::cup
{
	//Scene class that holds all the entities in the scene
	//Does not hold the camera, those are associated with views (probably players?)
	//The scene should hold data that is persistent across frames
    class Scene : public IUpdatable
    {
    public:
        Scene() = default;
        ~Scene() = default;

        void addGameObject(std::unique_ptr<GameObject> object);
		void collectRenderables(brew::RenderCommandCollector& collector) const;
		void collectLights(brew::RenderCommandCollector& collector) const;

		template<typename T>
        std::vector<GameObject*> getObjectsWithComponent() const
        {
            std::vector<GameObject*> objects;
            for (const auto& object : m_objects)
            {
                if (object->getComponent<T>() != nullptr)
                {
                    objects.push_back(object.get());
                }
            }
            return objects;
		}

        void update(double deltaTime) override;
    private:

        std::vector<std::unique_ptr<GameObject>> m_objects;

    private:
    };
}