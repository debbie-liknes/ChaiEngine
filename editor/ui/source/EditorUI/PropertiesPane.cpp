#include <EditorUI/PropertiesPane.h>
#include <UI/Core/InternalChaiUI.h>
#include <TypeRegistry.h>
#include <UI/Core/TypeWidgets.h>

namespace chai::ui
{
    void drawPropertiesPane(scene::Object* obj)
    {
        if (!obj) {
            Text("Select something u bitch");
            return;
        }

        auto typeInfo = obj->typeInfo();
        if (!typeInfo) {
            Text("Select something different u bitch");
            return;
        }

        for (const auto& [name, propInfo] : typeInfo->properties) {
            drawProperty(obj, propInfo);
        }
    }
}