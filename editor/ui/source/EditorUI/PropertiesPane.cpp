#include <EditorUI/PropertiesPane.h>
#include <UI/Core/InternalChaiUI.h>
#include <TypeRegistry.h>
#include <UI/Core/TypeWidgets.h>

namespace chai::ui
{
    void drawPropertiesPane(scene::Object* obj)
    {
        if (!obj) {
            Text("Select Something");
            return;
        }

        auto typeInfo = obj->typeInfo();
        if (!typeInfo) {
            return;
        }

        for (const auto& [name, propInfo] : typeInfo->properties) {
            drawProperty(obj, propInfo);
        }
    }
}