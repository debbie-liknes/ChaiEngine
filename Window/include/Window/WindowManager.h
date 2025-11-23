#pragma once
#include <WindowModuleExport.h>
#include <Window/Window.h>
#include <memory>
#include <vector>

namespace chai
{
class WindowSystem;

// Window manager - high-level interface for the engine
class WINDOWMODULE_EXPORT WindowManager
{
public:
    WindowManager(std::shared_ptr<WindowSystem> system);
    ~WindowManager();

    WindowManager(const WindowManager&) = delete;
    WindowManager& operator=(const WindowManager&) = delete;

    WindowManager() = default;
    WindowManager(WindowManager&&) = default;
    WindowManager& operator=(WindowManager&&) = default;

    WindowId createWindow(const WindowDesc& desc);
    void requestClose(WindowId id);
    bool isDone() const;

    void update();

private:
    std::shared_ptr<WindowSystem> m_windowSystem;
    std::vector<std::unique_ptr<Window>> m_windows;

    Window* findWindowById(WindowId id);
    //std::vector<std::unique_ptr<WindowEventHandler>> m_globalHandlers;
};
}