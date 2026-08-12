#include <Components/MeshComponent.h>
#include <Components/TransformComponent.h>
#include <Scene/GameObject.h>

namespace chai::scene
{
    MeshComponent::MeshComponent(GameObject* owner) : Component(owner)
    {

    }

    MeshComponent::~MeshComponent()
    {

    }
}

CHAI_REFLECT(chai::scene::MeshComponent, "MeshComponent") {
    CHAI_FIELD(mesh_);
    CHAI_FIELD(material_);
}

