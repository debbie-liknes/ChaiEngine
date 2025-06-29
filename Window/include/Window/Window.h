#pragma once
#include <WindowModuleExport.h>
#include <string>
#include <vector>
#include <memory>
#include <Core/InputState.h>

namespace chai 
{
    class Window;
    class WindowManager;
    using WindowId = uint64_t;

    //use this structure for window platform user pointer
    struct WindowData
    {
        Window* window;
        WindowManager* manager;
    };

    class Viewport;
	struct ViewportDesc;
	class WindowSystem;

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
    };


    // Individual window class
    class WINDOWMODULE_EXPORT Window
    {
		friend class WindowManager;
    public:
        Window();
        Window(const WindowDesc& desc);
        virtual ~Window();
        //enum class State { Created, Initialized, Destroyed };

		WindowId getId() const { return m_id; }

		void setWindowData(WindowData data) { m_data = data; }
		WindowData& getWindowData() { return m_data; }
		void setNativeWindow(void* handle) { nativeWindow = handle; }

    private:
		WindowId m_id{ 0 };
        WindowDesc m_desc;
        WindowData m_data;
		void* nativeWindow{ nullptr };
    };
}