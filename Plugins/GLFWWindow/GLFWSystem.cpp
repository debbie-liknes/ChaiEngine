#include "GLFWSystem.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <Window/Window.h>
#include <Window/WindowManager.h>
#include <Input/InputSystem.h>

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
#elif defined(__linux__)
#define GLFW_EXPOSE_NATIVE_X11
#elif defined(__APPLE__)
#define GLFW_EXPOSE_NATIVE_COCOA
#endif

#include <GLFW/glfw3native.h>

namespace chai
{
    void glfwCloseCallback(GLFWwindow* window)
    {
        auto userPtr = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        if (userPtr)
        {
            userPtr->manager->requestClose(userPtr->window->getId());
        }
    }

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        auto userPtr = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        if (userPtr)
        {
            if (action == GLFW_PRESS || action == GLFW_REPEAT)
            {
                InputSystem::instance().queueEvent(std::make_unique<KeyPressEvent>(userPtr->window->getId(), key));
            }
            else if (action == GLFW_RELEASE)
            {
                InputSystem::instance().queueEvent(std::make_unique<KeyReleaseEvent>(userPtr->window->getId(), key));
            }
        }
    }

    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    {
        auto userPtr = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        if (userPtr)
        {
            if (action == GLFW_PRESS)
            {
                InputSystem::instance().queueEvent(
                    std::make_unique<MouseDownEvent>(userPtr->window->getId(), static_cast<uint32_t>(button)));
            }
            else if (action == GLFW_RELEASE)
            {
                InputSystem::instance().queueEvent(
                    std::make_unique<MouseUpEvent>(userPtr->window->getId(), static_cast<uint32_t>(button)));
            }
        }
    }

    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
    {
        auto userPtr = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        if (userPtr)
        {
            InputSystem::instance().queueEvent(std::make_unique<MouseMoveEvent>(userPtr->window->getId(), xpos, ypos));
        }
    }

    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        auto userPtr = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        if (userPtr)
        {
            InputSystem::instance().queueEvent(
                std::make_unique<FrameBufferResize>(userPtr->window->getId(), width, height));
        }
    }

    GLFWSystem::GLFWSystem()
    {
    }

    GLFWSystem::~GLFWSystem()
    {
    }

    bool GLFWSystem::initialize()
    {
        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW!" << std::endl;
            return false;
        }

        return true;
    }

    void GLFWSystem::shutdown()
    {
        glfwTerminate();
    }

    // Window management
    std::unique_ptr<Window> GLFWSystem::createWindow(const WindowDesc& desc, WindowManager* manager)
    {
        auto window = std::make_unique<Window>(desc);
        auto glfwWindow = glfwCreateWindow(desc.width, desc.height, desc.title.c_str(), NULL, NULL);
        if (!glfwWindow)
        {
            std::cerr << "Failed to create GLFW Window!" << std::endl;
        }

        glfwSetWindowCloseCallback(glfwWindow, glfwCloseCallback);
        glfwSetKeyCallback(glfwWindow, key_callback);
        glfwSetMouseButtonCallback(glfwWindow, mouse_button_callback);
        glfwSetCursorPosCallback(glfwWindow, cursor_position_callback);
        glfwSetFramebufferSizeCallback(glfwWindow, framebuffer_size_callback);

        window->setWindowData(WindowData{window.get(), manager});
        window->setSystemWindow(glfwWindow);
        glfwSetWindowUserPointer(glfwWindow, &window->getWindowData());

        //is this opengl specific?
        //glfwMakeContextCurrent(glfwWindow);
        //glfwMakeContextCurrent(NULL);

        return std::move(window);
    }

    void GLFWSystem::pollEvents()
    {
        glfwPollEvents();
    }

    GLFWwindow* GLFWSystem::convertToGLFWWindow(void* nativeWindow)
    {
        return static_cast<GLFWwindow*>(nativeWindow);
    }

    void GLFWSystem::destroyWindow(void* nativeWindow)
    {
        if (auto glfwWindow = convertToGLFWWindow(nativeWindow); glfwWindow)
        {
            glfwDestroyWindow(glfwWindow);
        }
    }

    void* GLFWSystem::getProcAddress()
    {
        return glfwGetProcAddress;
    }

    void GLFWSystem::swapBuffers(void* nativeWindow)
    {
        glfwSwapBuffers(convertToGLFWWindow(nativeWindow));
    }

    void GLFWSystem::destroyAllWindows()
    {
    }

    std::unique_ptr<RenderSurface> GLFWSystem::createRenderSurface(void* window)
    {
        return std::make_unique<GlfwRenderSurface>(convertToGLFWWindow(window));
    }


    void GlfwRenderSurface::makeCurrent()
    {
        glfwMakeContextCurrent(m_window);
    }

    void GlfwRenderSurface::doneCurrent()
    {
        //This doesnt work the way I expect
        //glfwMakeContextCurrent(nullptr);

#ifdef _WIN32
        wglMakeCurrent(NULL, NULL);
#elif defined(__linux__)
        glXMakeCurrent(glXGetCurrentDisplay(), None, NULL);
#endif
    }

    void GlfwRenderSurface::swapBuffers()
    {
        glfwSwapBuffers(m_window);
    }

    int GlfwRenderSurface::getWidth() const
    {
        int w, h;
        glfwGetFramebufferSize(m_window, &w, &h);
        return w;
    }

    int GlfwRenderSurface::getHeight() const
    {
        int w, h;
        glfwGetFramebufferSize(m_window, &w, &h);
        return h;
    }
}

CHAI_PLUGIN_SERVICES(GLFWWindowPlugin)
{
    CHAI_SERVICE_AS(chai::WindowSystem, chai::GLFWSystem, "WindowSystem");
}

CHAI_DEFINE_PLUGIN_ENTRY(GLFWWindowPlugin, "GLFWWindowPlugin", "1.0.0", GLFWWINDOWPLUGIN_EXPORT)