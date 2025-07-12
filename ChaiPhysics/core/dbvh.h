#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <span>
#include <array>
#include <memory>

//
// \brief Dynamic Bounding Volume Hierarchy
//
// \default Uses a Surface Area Heuristic (SAH) to group scene objects.
//
// Objects are re-inserted when they move.
//

// TODO: Probably need some sort of pool allocator for object
// insertion / deletion / re-insertion.
namespace chai
{
    // NOTE: Bullet uses two dbvh's:
    // 1. Dynamic scene objects
    // 2. Static scene objects
    // Does it query both separately? How do static and dynamic meshes interact?
    // Being able to filter out all static meshes for collision updates seems
    // really attractive.
    class dbvh
    {
    public:        
        // Axis-aligned Bounding Box
        struct aabb
        {
            bool operator==(const aabb&) const;

            double getSurfaceArea() const;
            aabb expand(const aabb& box) const;

            // The center point in the AABB
            glm::vec3 center;
            double width;
            double length;
            double height;
        };

        struct BoxCollider
        {
            aabb box;

            // Local rotation is represented by centroid starting point
            // multiplied by quaternion for rotational axis + degree.
            glm::vec3 center;
            glm::quat rot;

            glm::vec4 rotateAt(glm::vec3 pos) const
            {
                return rot * glm::vec4(pos, 1.) * glm::inverse(rot);
            }

            aabb getWorldBounds() const;
        };

        struct Node
        {
            Node* left = nullptr;
            Node* right = nullptr;
            Node* parent = nullptr;

            // An enlarged bounding box is used to prevent constant
            // re-insertion due to the small movements of objects.
            aabb boundsEnlarged;

            // This axis-aligned bounding box isn't necessarily
            // the bounding box around a GameObject. This is the
            // world space bounding box.
            aabb boundsLeaf;

            // TODO: Add this in once Debbie's things are merged.
            // How does the GameObject get picked up by the physics
            // engine? Does every GameObject necessarily have a
            // collider? Is there a thing that says this object is
            // interactable by the physics engine?
            //GameObject* object;
            const BoxCollider* object; // until the above comes in
        };

        dbvh();
        ~dbvh();

        Node* getRootNode() const;

        void update();
        void insert(const BoxCollider* object);
        void remove(const BoxCollider* object);

        std::span<aabb> getOverlappingPairs() const;

    private:
        // Non-copyable (don't have a serialization process of the tree).
        dbvh(const dbvh&) = delete;
        const dbvh& operator=(const dbvh&) = delete;

        class impl;
        std::unique_ptr<impl> m_p;
    };
}
