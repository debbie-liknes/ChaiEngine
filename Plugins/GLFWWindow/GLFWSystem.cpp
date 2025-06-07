#include "GLFWSystem.h"

namespace chai
{
	GLFWSystem::GLFWSystem()
	{

	}

	GLFWSystem::~GLFWSystem()
	{
	}

    bool GLFWSystem::initialize() { return true; }
    void GLFWSystem::shutdown(){}
    void GLFWSystem::update(){}

    // Window management
    Window* GLFWSystem::createWindow(const WindowDesc& desc) { return nullptr; }
    Window* GLFWSystem::createWindow(const std::string& title, int width, int height){ return nullptr; }
    Window* GLFWSystem::getWindow(const std::string& name){ return nullptr; }
    void GLFWSystem::destroyWindow(const std::string& name){}
    void GLFWSystem::destroyAllWindows(){}

    // Properties
    //const std::vector<std::unique_ptr<Window>>& GLFWSystem::getWindows() const { return m_windows; }
    //size_t GLFWSystem::getWindowCount() const { return m_windows.size(); }
    //bool GLFWSystem::hasWindows() const { return !m_windows.empty(); }
    //bool GLFWSystem::shouldClose() const;

    // Input system integration
    //void setInputSystem(InputSystem* inputSystem);
    //InputSystem* getInputSystem() { return m_inputSystem; }

    // Internal
    //Window* GLFWSystem::getWindowByHandle(GLFWwindow* handle) {}
}