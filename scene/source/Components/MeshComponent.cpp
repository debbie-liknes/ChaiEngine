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

    CHAI_ICON(ICON_FA_CUBES);
    CHAI_PROPERTY("Mesh", getMesh, setMesh);
    CHAI_PROPERTY("Material", getMaterial, setMaterial);
}

