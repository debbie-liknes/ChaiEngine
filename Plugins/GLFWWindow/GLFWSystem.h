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

        bool initialize();
        void shutdown();
        void update();

        // Window management
        Window* createWindow(const WindowDesc& desc);
        Window* createWindow(const std::string& title, int width = 1280, int height = 720);
        Window* getWindow(const std::string& name);
        void destroyWindow(const std::string& name);
        void destroyAllWindows();

        // Properties
        //const std::vector<std::unique_ptr<Window>>& getWindows() const { return m_windows; }
        //size_t getWindowCount() const { return m_windows.size(); }
        //bool hasWindows() const { return !m_windows.empty(); }
        //bool shouldClose() const;

        // Input system integration
        //void setInputSystem(InputSystem* inputSystem);
        //InputSystem* getInputSystem() { return m_inputSystem; }

        // Internal
        //Window* getWindowByHandle(GLFWwindow* handle);

    private:
        // GLFW callbacks (static)
        //static void glfwErrorCallback(int error, const char* description);
        //static void glfwWindowSizeCallback(GLFWwindow* window, int width, int height);
        //static void glfwFramebufferSizeCallback(GLFWwindow* window, int width, int height);
        //static void glfwWindowCloseCallback(GLFWwindow* window);
        //static void glfwWindowFocusCallback(GLFWwindow* window, int focused);
	};
}

CHAI_PLUGIN_CLASS(GLFWWindowPlugin) {
    CHAI_SERVICE(chai::GLFWSystem, "GLFWSystem");
    //TODO: if there are other loaders, does there need to be a manager for them?
}

CHAI_REGISTER_PLUGIN(GLFWWindowPlugin, "GLFWWindowPlugin", "1.0.0")