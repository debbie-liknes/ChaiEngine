#pragma once

#include <cstdint>

namespace chai::scene
{
    class GameObject;
    class Component;

    using Mask = uint64_t;
    constexpr Mask MASK_OFF = 0UL;
    constexpr Mask MASK_ALL = ~MASK_OFF;

    /*
    * @brief Double-dispatch pattern for visiting nodes within the scene.
    */
    class Visitor
    {
    public:
        virtual ~Visitor() = default;

        /*
        * @brief Used to reset internal state if the visitor accumulates state.
        */
        virtual void reset() {}

        /*
        * @brief Callback for an updatable object (GameObject, Component).
        */
        virtual void visit(GameObject* node) = 0;
        virtual void visit(Component* node) = 0;

        /*
        * @brief Set mask, filtering out any nodes that don't fit the mask.
        */
        void setMask(Mask mask)
        {
            traversalMask_ = mask;
        }

    private:
        Mask traversalMask_ = MASK_ALL;
    };
}
