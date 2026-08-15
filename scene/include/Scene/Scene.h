#pragma once
#include <vector>
#include <Scene/GameObject.h>
#include <Scene/IUpdatable.h>

namespace chai::scene
{
    //Scene class that holds all the entities in the scene
    //Does not hold the camera, those are associated with views (probably players?)
    //The scene should hold data that is persistent across frames
    class Scene
    {
    public:
        Scene();
        ~Scene() = default;

        // Disable copying (engine should have exclusive ownership)
        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        // Disable moving (engine should have exclusive ownership)
        Scene(Scene&&) = delete;
        Scene& operator=(Scene&&) = delete;

        void update(const UpdateContext&);
        void accept(Visitor* visitor);

        std::vector<std::shared_ptr<GameObject>>& getObjects() { return m_objects; }

        GameObject* createObject(const std::string& name);
        void setCamera(GameObject* cam);
        ObjectId getCameraId() const { return camera_->id(); }
        void setLight(GameObject* sun);

        template <typename T>
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


    private:
        std::vector<std::shared_ptr<GameObject>> m_objects;

        //special objects - but i dont really like that they are special
        GameObject* sun_ = nullptr;
        GameObject* camera_ = nullptr;
    };
}