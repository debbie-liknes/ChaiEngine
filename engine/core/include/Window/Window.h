/**
 * @file Window.h
 * @brief Interface for a generic window (glfw, sdl, etc)
 */
#pragma once
#include <CoreExport.h>
#include <string>
#include <memory>
#include <span>
#include <Window/WindowTypes.h>

namespace chai
{
    class CORE_EXPORT IWindow
    {
    public:
        virtual ~IWindow() = default;

        /**
         * @brief Per frame event pump
         */
        virtual std::span<const WindowEvent> pollEvents() = 0;

        /**
         * @brief Query the close state
         */
        virtual bool shouldClose() const = 0;

        /**
         * @brief Request to close the window.
         * Will be processed when safe
         */
        virtual void requestClose() = 0;

        /**
         * @brief Screen coordinates
         */
        virtual void windowSize(int& w, int& h) const = 0;

        /**
         * @brief Pixels
         */
        virtual void framebufferSize(int& w, int& h) const = 0;

        /**
         * @brief Window title setter
         */
        virtual void setTitle(std::string_view title) = 0;

        /**
         * @brief Opaque
         */
        virtual void* nativeHandle() const = 0;
    };
    //class Window;
    //class WindowManager;
    //using WindowId = uint64_t;

    ////use this structure for window platform user pointer
    //struct WindowData
    //{
    //    Window* window;
    //    WindowManager* manager;
    //};

    //class Viewport;
    //struct ViewportDesc;
    //class WindowSystem;

    //// Window creation descriptor
    //struct WindowDesc
    //{
    //    std::string title = "Untitled Window";
    //    int width = 1280;
    //    int height = 720;
    //    int x = -1, y = -1; // -1 means center on screen
    //    bool resizable = true;
    //    bool decorated = true;
    //    bool fullscreen = false;
    //    bool vsync = true;
    //    int samples = 0; // MSAA samples
    //};


    //// Individual window class
    //class WINDOWMODULE_EXPORT Window
    //{
    //    friend class WindowManager;

    //public:
    //    Window();
    //    Window(const WindowDesc& desc);
    //    virtual ~Window();
    //    //enum class State { Created, Initialized, Destroyed };

    //    WindowId getId() const { return m_id; }

    //    void setWindowData(WindowData data) { m_data = data; }
    //    WindowData& getWindowData() { return m_data; }
    //    void setSystemWindow(void* handle) { systemWindow = handle; }
    //    void setNativeWindow(void* handle) { nativeWindow = handle; }
    //    void setNativeContext(void* ctx) { nativeContext = ctx; }

    //private:
    //    WindowId m_id{0};
    //    WindowDesc m_desc;
    //    WindowData m_data;
    //    void* systemWindow{nullptr};
    //    void* nativeWindow{nullptr};
    //    void* nativeContext{nullptr};
    //};

    //class RenderSurface
    //{
    //public:
    //    virtual ~RenderSurface() = default;

    //    // Must be called from the render thread before any GL calls.
    //    virtual void makeCurrent() = 0;

    //    // Optional; can be a no-op for GL.
    //    virtual void doneCurrent() = 0;

    //    // Called once per frame on the render thread after drawing.
    //    virtual void swapBuffers() = 0;

    //    // For viewport / camera setup.
    //    virtual int getWidth() const = 0;
    //    virtual int getHeight() const = 0;
    //};
}