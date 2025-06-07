#pragma once
#include <WindowModuleExport.h>
#include <vector>
#include <string>
#include <memory>

namespace chai 
{
    class Window;
    struct WindowDesc;
    // Main window system
    class WINDOWMODULE_EXPORT WindowSystem
    {
    public:
        WindowSystem();
        ~WindowSystem();

        virtual bool initialize() = 0;
        virtual void shutdown() = 0;
        virtual void update() = 0;

        // Window management
        virtual Window* createWindow(const WindowDesc& desc) = 0;
        virtual Window* createWindow(const std::string& title, int width = 1280, int height = 720) = 0;
        virtual Window* getWindow(const std::string& name) = 0;
        virtual void destroyWindow(const std::string& name) = 0;
        virtual void destroyAllWindows() = 0;

        // Properties
        //virtual const std::vector<std::unique_ptr<Window>>& getWindows() const = 0;// { return m_windows; }
        //virtual size_t getWindowCount() const = 0;// { return m_windows.size(); }
        //virtual bool hasWindows() const = 0;// { return !m_windows.empty(); }
        //virtual bool shouldClose() const = 0;

        // Input system integration
        //void setInputSystem(InputSystem* inputSystem);
        //InputSystem* getInputSystem() { return m_inputSystem; }

        // Internal
        //virtual Window* getWindowByHandle(void* handle) = 0;

    private:
        //std::vector<std::unique_ptr<Window>> m_windows;
        //std::unordered_map<std::string, Window*> m_windowsByName;
        //std::unordered_map<GLFWwindow*, Window*> m_windowsByHandle;

        //bool m_initialized;
        //InputSystem* m_inputSystem;
    };
}