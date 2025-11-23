#include <Window/Window.h>

namespace chai
{
    Window::Window(const WindowDesc& desc) : m_desc(desc), m_data({this})
    {
    }

    Window::Window() : m_data()
    {
        m_data.window = this;
    }

    Window::~Window()
    {
    }

    //bool Window::initialize()
    //{

    //}

    //void Window::destroy()
    //{

    //}

    //void Window::update()
    //{

    //}

    //// Window operations
    //void Window::makeContextCurrent()
    //{

    //}

    //void Window::swapBuffers()
    //{

    //}

    //void Window::setTitle(const std::string& title)
    //{

    //}

    //void Window::setSize(int width, int height)
    //{

    //}

    //void Window::setPosition(int x, int y)
    //{

    //}

    //void Window::setVSync(bool enabled)
    //{

    //}

    //void Window::setFullscreen(bool fullscreen)
    //{

    //}

    //// Viewport management
    //Viewport* Window::createViewport(const ViewportDesc& desc)
    //{

    //}

    //Viewport* Window::getViewport(const std::string& name)
    //{

    //}

    //void Window::removeViewport(const std::string& name)
    //{

    //}

    //// Event handling
    //void Window::addEventHandler(WindowEventHandler* handler)
    //{

    //}

    //void Window::removeEventHandler(WindowEventHandler* handler)
    //{

    //}

    //// Internal callback handlers
    //void Window::handleResize(int width, int height)
    //{

    //}

    //void Window::handleFramebufferResize(int width, int height)
    //{

    //}

    //void Window::handleClose()
    //{

    //}

    //void Window::handleFocus(bool focused)
    //{

    //}
}