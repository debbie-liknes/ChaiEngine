#pragma once
#include <unordered_map>
#include <TypeRegistry.h>
#include <TypeInfo.h>
#include <IconsFontAwesome7.h>

namespace chai::ui
{
    using IconLookup = std::unordered_map<std::type_index, const char*>;

    IconLookup& iconRegistry();

    template <typename T>
    void registerIcon(const char* icon)
    {
        iconRegistry()[std::type_index(typeid(T))] = icon;
    }

    const char* iconForComponent(TypeInfo& type);
}