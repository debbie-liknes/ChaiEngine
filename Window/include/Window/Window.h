#pragma once
#include <WindowModuleExport.h>
#include <string>
#include <vector>
#include <memory>

namespace chai 
{
    class Viewport;
	struct ViewportDesc;
	class WindowSystem;
	//class WindowEventHandler;

    // Window creation descriptor
    struct WINDOWMODULE_EXPORT WindowDesc
    {
        std::string title = "Untitled Window";
        int width = 1280;
        int height = 720;
        int x = -1, y = -1;  // -1 means center on screen
        bool resizable = true;
        bool decorated = true;
        bool fullscreen = false;
        bool vsync = true;
        int samples = 0;  // MSAA samples

        // OpenGL context settings
        int majorVersion = 4;
        int minorVersion = 5;
        bool coreProfile = true;
    };

    // Individual window class
    class WINDOWMODULE_EXPORT Window
    {
    public:
        enum class State { Created, Initialized, Destroyed };

    public:
        Window(const WindowDesc& desc, WindowSystem* windowSystem);
        ~Window();

        //bool initialize();
        //void destroy();
        //void update();

        //// Window operations
        //void makeContextCurrent();
        //void swapBuffers();
        //void setTitle(const std::string& title);
        //void setSize(int width, int height);
        //void setPosition(int x, int y);
        //void setVSync(bool enabled);
        //void setFullscreen(bool fullscreen);

        //// Viewport management
        //Viewport* createViewport(const ViewportDesc& desc);
        //Viewport* getViewport(const std::string& name);
        //void removeViewport(const std::string& name);
        //const std::vector<std::unique_ptr<Viewport>>& getViewports() const { return m_viewports; }

        //// Event handling
        //void addEventHandler(WindowEventHandler* handler);
        //void removeEventHandler(WindowEventHandler* handler);

        // Properties
        //const WindowDesc& getDesc() const { return m_desc; }
        //void* getHandle() { return m_handle; }
        //State getState() const { return m_state; }
        //bool shouldClose() const;
        //int getWidth() const { return m_desc.width; }
        //int getHeight() const { return m_desc.height; }
        //int getFramebufferWidth() const { return m_framebufferWidth; }
        //int getFramebufferHeight() const { return m_framebufferHeight; }

        // Internal callback handlers
        //void handleResize(int width, int height);
        //void handleFramebufferResize(int width, int height);
        //void handleClose();
        //void handleFocus(bool focused);

    private:
        WindowDesc m_desc;
        State m_state;
        WindowSystem* m_windowSystem;

        //std::vector<std::unique_ptr<Viewport>> m_viewports;
        //std::vector<WindowEventHandler*> m_eventHandlers;

        // OpenGL context info
        int m_framebufferWidth, m_framebufferHeight;
    };

    // Window event callbacks
    class WindowEventHandler
    {
    public:
        virtual ~WindowEventHandler() = default;
        virtual void onWindowResize(Window* window, int width, int height) {}
        virtual void onWindowClose(Window* window) {}
        virtual void onWindowFocus(Window* window, bool focused) {}
        virtual void onFramebufferResize(Window* window, int width, int height) {}
    };
}