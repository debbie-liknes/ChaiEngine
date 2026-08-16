#include <Assets/MaterialAsset.h>
#include <Assets/MeshAsset.h>
#include <Assets/TextureAsset.h>
#include <ChaiMath.h>
#include <Handle.h>
#include <UI/Core/InternalChaiUI.h>
#include <UI/Core/TypeWidgets.h>

namespace chai::ui
{
    template <typename T>
    void drawHandleProperty()
    {
    }

    void drawProperty(void* object, const TypeInfo::PropertyInfo& property)
    {
        std::any val = property.getter(object);

        if (property.type == typeid(float)) {
            auto v = std::any_cast<float>(val);
            if (DragFloat(property.name, &v)) {
                property.setter(object, v);
            }
        } else if (property.type == typeid(math::Vec3)) {
            auto v = std::any_cast<math::Vec3>(val);
            if (DragFloatVec3(property.name, v)) {
                property.setter(object, v);
            }
        } else if (property.type == typeid(Handle<gfx::Mesh>)) {
            auto v = std::any_cast<Handle<gfx::Mesh>>(val);

        } else if (property.type == typeid(Handle<gfx::Material>)) {
            auto v = std::any_cast<Handle<gfx::Material>>(val);
        }
    }
} // namespace chai::ui