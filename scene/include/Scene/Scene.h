#pragma once
#include <vector>
#include <Scene/GameObject.h>
#include <Core/Updatable.h>
#include <Scene/IScene.h>

namespace chai::scene
{
    //Scene class that holds all the entities in the scene
    //Does not hold the camera, those are associated with views (probably players?)
    //The scene should hold data that is persistent across frames
    class Scene : public IScene
    {
    public:
        Scene() = default;
        ~Scene() = default;

        void update(const UpdateContext&) override;
        void extract(gfx::FrameRenderData& frame) const override;
        uint32_t getCameraId() const override { return camera_->getObjectId(); }

        GameObject* createObject(const std::string& name);
        void setCamera(GameObject* cam);
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
        class ObjIdAllocator
        {
        public:
            GameObjectId allocate() { return next_++; }

        private:
            GameObjectId next_ = 0;
        };

        ObjIdAllocator gameObjAllocator_;

        std::vector<std::shared_ptr<GameObject>> m_objects;

        //special objects - but i dont really like that they are special
        GameObject* sun_;
        GameObject* camera_;
    };
}