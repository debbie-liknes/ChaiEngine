#include <UI/Core/TypeWidgets.h>
#include <UI/Core/InternalChaiUI.h>
#include <ChaiMath.h>

namespace chai::ui
{
    void drawProperty(void* object, const TypeInfo::PropertyInfo& property)
    {
        std::any val = property.getter(object);

        if (property.type == typeid(math::Vec3)) {
            auto v = std::any_cast<math::Vec3>(val);
            if (DragFloatVec3(property.name, v))
            {
                property.setter(object, v);
            }
        }
    }
}