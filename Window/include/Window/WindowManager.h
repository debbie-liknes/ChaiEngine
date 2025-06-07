#pragma once
#include <WindowModuleExport.h>
#include <Window/Window.h>
#include <string>
#include <memory>

namespace chai
{
	class WindowSystem;
    // Window manager - high-level interface for the engine
    class WINDOWMODULE_EXPORT WindowManager
    {
    public:
        WindowManager(std::unique_ptr<WindowSystem> system);
        ~WindowManager();

        //bool initialize();
        //void shutdown();
        //void update();

        // Advanced window creation
        //Window* createWindow(const WindowDesc& desc);
        //Window* getWindow(const std::string& name);

        // Global event handling
        //void addGlobalEventHandler(std::unique_ptr<WindowEventHandler> handler);

        // Properties
        //WindowSystem* getWindowSystem() { return m_windowSystem.get(); }
        //bool shouldClose() const;

    private:
        std::unique_ptr<WindowSystem> m_windowSystem;
        //std::vector<std::unique_ptr<WindowEventHandler>> m_globalHandlers;
    };
}