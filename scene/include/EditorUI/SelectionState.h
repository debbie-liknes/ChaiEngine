#pragma once
#include <functional>

namespace chai::scene
{
    class GameObject;
}

namespace chai::ui
{
    class SelectionState
    {
    public:
        void select(scene::GameObject* obj) { selected_ = obj; }
        void clear() { selected_ = nullptr; }
        scene::GameObject* selected() const { return selected_; }

    private:
        scene::GameObject* selected_ = nullptr;
    };
} // namespace chai::ui