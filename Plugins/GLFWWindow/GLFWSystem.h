#pragma once
#include <GLFWWindowPluginExport.h>
#include <Window/WindowSystem.h>
#include <Window/Window.h>
#include <Meta/ChaiMacros.h>

struct GLFWwindow;

namespace chai
{
    class GLFWWINDOWPLUGIN_EXPORT GLFWSystem final : public WindowSystem
    {
    public:
        GLFWSystem();
        ~GLFWSystem() override;

        bool initialize() override;
        void shutdown() override;
        void pollEvents() override;

        std::unique_ptr<Window> createWindow(const WindowDesc& desc, WindowManager* manager) override;
        void destroyWindow(void* nativeWindow) override;
        void destroyAllWindows() override;
        void swapBuffers(void* nativeWindow) override;

        void* getProcAddress() override;
        std::unique_ptr<RenderSurface> createRenderSurface(void* window) override;


    private:
        GLFWwindow* convertToGLFWWindow(void* nativeWindow);
    };

    class GlfwRenderSurface : public RenderSurface
    {
    public:
        explicit GlfwRenderSurface(GLFWwindow* window) : m_window(window) {}

        void makeCurrent() override;

        void doneCurrent() override;

        void swapBuffers() override;

        int getWidth() const override;

        int getHeight() const override;

    private:
        GLFWwindow* m_window;
    };

}

CHAI_PLUGIN_CLASS (GLFWWindowPlugin)
{
    CHAI_SERVICE_AS(chai::WindowSystem, chai::GLFWSystem, "WindowSystem");
}

CHAI_REGISTER_PLUGIN(GLFWWindowPlugin, "GLFWWindowPlugin", "1.0.0")