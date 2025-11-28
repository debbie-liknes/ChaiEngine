#pragma once
#include <WindowModuleExport.h>
#include <memory>

namespace chai
{
    class Window;
    struct WindowDesc;
    class WindowManager;
    class RenderSurface;

    // Main window system
    class WINDOWMODULE_EXPORT WindowSystem
    {
    public:
        WindowSystem();

        virtual ~WindowSystem();
        WindowSystem(const WindowSystem&) = delete;

        virtual bool initialize() = 0;
        virtual void shutdown() = 0;

        virtual void pollEvents() = 0;

        virtual void* getProcAddress() = 0;

        virtual void swapBuffers(void* nativeWindow)
        {
        }

        // Window management
        virtual std::unique_ptr<Window> createWindow(const WindowDesc& desc, WindowManager* manager) =
        0;
        virtual void destroyWindow(void* nativeWindow) = 0;
        virtual void destroyAllWindows() = 0;

        virtual std::unique_ptr<RenderSurface> createRenderSurface(void* window) = 0;
    };
}