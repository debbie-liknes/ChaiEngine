#pragma once
#include <UICoreExport.h>
#include <string>

namespace chai::ui
{
    struct UICORE_EXPORT ButtonDesc {
        std::string label;
        std::string id;
    };

    bool UICORE_EXPORT button(const std::string& label);    //simple wrapper
    bool UICORE_EXPORT button(const ButtonDesc& desc);
    bool UICORE_EXPORT checkbox(const std::string& label, bool* val);
} // namespace chai::ui