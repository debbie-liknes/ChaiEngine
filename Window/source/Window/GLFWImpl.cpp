#include <Window/GLFWImpl.h>
#include <iostream>

namespace CGraphics
{
    ChaiWindow::ChaiWindow(Core::CString title, int width, int height) : Window(title, width, height)
    {
        Init();
    }

    ChaiWindow::~ChaiWindow()
    {
        glfwTerminate();
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

    CGraphics::Window::WindowProc ChaiWindow::getProcAddress()
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