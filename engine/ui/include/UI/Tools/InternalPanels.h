#pragma once
#include <UIExport.h>
#include <functional>
#include <string>

namespace chai::ui
{
    using PanelDrawFn = std::function<void()>;

    UI_EXPORT void registerPanel(const std::string& name, PanelDrawFn fn, bool startVisible = true);
    UI_EXPORT void unregisterPanel(const std::string& name);
    UI_EXPORT void setPanelVisible(const std::string& name, bool visible);

    // called once per frame by Engine, between NewFrame() and EndFrame()
    UI_EXPORT void drawRegisteredPanels();
} // namespace chai::ui