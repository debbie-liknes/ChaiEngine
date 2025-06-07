#pragma once
#include <GLFWWindowPluginExport.h>
#include <Window/WindowSystem.h>
#include <Meta/ChaiMacros.h>

struct GLFWwindow;
namespace chai
{
	class GLFWWINDOWPLUGIN_EXPORT GLFWSystem : public WindowSystem
	{
    public:
        GLFWSystem();
        ~GLFWSystem();

        bool initialize() override;
        void shutdown() override;
        void pollEvents() override;

        std::unique_ptr<Window> createWindow(const WindowDesc& desc, WindowManager* manager) override;
        void destroyWindow(void* nativeWindow) override;
        void destroyAllWindows() override;

    private:
		GLFWwindow* convertToGLFWWindow(void* nativeWindow);
	};
}

CHAI_PLUGIN_CLASS(GLFWWindowPlugin) {
    CHAI_SERVICE_AS(chai::WindowSystem, chai::GLFWSystem, "WindowSystem");
}

CHAI_REGISTER_PLUGIN(GLFWWindowPlugin, "GLFWWindowPlugin", "1.0.0")