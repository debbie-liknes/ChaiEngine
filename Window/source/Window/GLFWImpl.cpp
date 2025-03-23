#include <Window/GLFWImpl.h>
#include <iostream>

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
        InputState* state = static_cast<InputState*>(userPtr);
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

    void ChaiWindow::Init()
    {
        //call init on the graphics api instead of doing this here
        //if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        //{
        //    std::cerr << "Failed to initialize GLAD" << std::endl;
        //}

        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW!" << std::endl;
        }

        m_window = glfwCreateWindow(m_width, m_height, m_title.data(), NULL, NULL);
        if (!m_window)
        {
            std::cerr << "Failed to create GLFW Window!" << std::endl;
        }

        glfwSetWindowUserPointer(m_window, &m_state);

        //callbacks
        glfwSetCursorPosCallback(m_window, mouse_callback);
        glfwSetKeyCallback(m_window, key_callback);

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