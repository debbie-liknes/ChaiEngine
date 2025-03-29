#include <Window/GLFWImpl.h>
#include <iostream>
#include <Window/Viewport.h>

namespace chai
{
    ChaiWindow::ChaiWindow(chai::CString title, int width, int height) : Window(title, width, height)
    {
        Init();
    }

    ChaiWindow::~ChaiWindow()
    {
        glfwTerminate();
    }

    //callbacks
    void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
        void* userPtr = glfwGetWindowUserPointer(window);
        InputState* state = static_cast<InputState*>(userPtr);
        if (!state) return;

        state->mousePos = { xpos, ypos };
    }

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        void* userPtr = glfwGetWindowUserPointer(window);
        GLFWInfo* info = static_cast<GLFWInfo*>(userPtr);
        if (!info) return;

        auto state = info->input;
        if (!state) return;

        if ((action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            state->keys.insert((Key)key);
        }
        else
        {
            state->keys.erase((Key)key);
        }
    }

    void resize_callback(GLFWwindow* window, int width, int height) {
        void* userPtr = glfwGetWindowUserPointer(window);
        GLFWInfo* info = static_cast<GLFWInfo*>(userPtr);
        if (!info) return;

        auto appWindow = info->window;
        if (!appWindow) return;

        appWindow->Resize(width, height);
    }

    void ChaiWindow::Init()
    {
        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW!" << std::endl;
        }

        m_window = glfwCreateWindow(m_width, m_height, m_title.data(), NULL, NULL);
        if (!m_window)
        {
            std::cerr << "Failed to create GLFW Window!" << std::endl;
        }

        m_userPointer.input = &m_state;
        m_userPointer.window = this;
        glfwSetWindowUserPointer(m_window, &m_userPointer);

        //callbacks
        glfwSetCursorPosCallback(m_window, mouse_callback);
        glfwSetKeyCallback(m_window, key_callback);
        glfwSetFramebufferSizeCallback(m_window, resize_callback);

        glfwMakeContextCurrent(m_window);
    }

    //Display Window
    bool ChaiWindow::Show()
    {
        if (!m_window) return false;

        return !glfwWindowShouldClose(m_window);

    }

    void ChaiWindow::Close()
    {
        glfwDestroyWindow(m_window);
    }

    void ChaiWindow::Resize(int width, int height)
    {
        //this WILL NOT work for multiple viewports. Need a split screen "rule"
        for (auto vp : m_viewports)
        {
            vp->setDimensions(0, 0, width, height);
        }
    }

    chai::Window::WindowProc ChaiWindow::getProcAddress()
    {
        return (WindowProc)glfwGetProcAddress;
    }

    // Input handling
    void ChaiWindow::PollEvents()
    {
        glfwPollEvents();
    }

    void ChaiWindow::swapBuffers()
    {
        glfwSwapBuffers(m_window);
    }
}