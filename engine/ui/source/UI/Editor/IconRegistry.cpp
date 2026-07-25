#include <UI/Editor/IconRegistry.h>

namespace chai::ui
{
    IconLookup& iconRegistry()
    {
        static IconLookup registry;
        return registry;
    }

    const char* iconForComponent(TypeInfo& type)
    {
        auto& reg = iconRegistry();
        auto it = reg.find(type.typeIndex);
        return it != reg.end() ? it->second : ICON_FA_QUESTION;
    }
}