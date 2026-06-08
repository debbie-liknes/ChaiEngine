#include "WindowGLFW.h"
#include <Log.h>

namespace chai
{
    WindowGLFW::WindowGLFW(const WindowDesc& desc)
    {
        // There might be a more graphics api agnostic way to do this
        // But ive got my heart set on vulkan right now, so this is how I will do it
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, desc.resizable ? GLFW_TRUE : GLFW_FALSE);

        window_ = glfwCreateWindow(desc.width, desc.height, desc.title.c_str(), nullptr, nullptr);
        if (!window_) {
            CHAI_LOG_CRITICAL("Failed to create GLFW window");
            return;
        }

        // Stash `this` so the static callbacks below can find us.
        //glfwSetWindowUserPointer(window_, this);
        //glfwSetFramebufferSizeCallback(window_, &WindowGLFW::onFramebufferSize);
        //glfwSetWindowCloseCallback(window_, &WindowGLFW::onClose);
        //glfwSetWindowFocusCallback(window_, &WindowGLFW::onFocus);

        //events_.reserve(16);

        //int fbw = 0, fbh = 0;
        //glfwGetFramebufferSize(window_, &fbw, &fbh);
        //CHAI_LOG_INFO(
        //    "Window created: {}x{} screen, {}x{} framebuffer", desc.width, desc.height, fbw, fbh);

    }

    WindowGLFW::~WindowGLFW()
    {
        if (window_)
            glfwDestroyWindow(window_);
    }


    std::span<const WindowEvent> WindowGLFW::pollEvents()
    {
        events_.clear();  // keeps capacity ? steady-state no allocation
        glfwPollEvents(); // invokes the callbacks, which pushEvent() into events_
        return events_;

    }

    bool WindowGLFW::shouldClose() const
    {
        return window_ ? glfwWindowShouldClose(window_) : true;
    }

    void WindowGLFW::requestClose()
    {
        if (window_)
            glfwSetWindowShouldClose(window_, GLFW_TRUE);
    }

    void WindowGLFW::windowSize(int& w, int& h) const
    {
        w = 0;
        h = 0;
        if (window_)
            glfwGetWindowSize(window_, &w, &h);

    }

    void WindowGLFW::framebufferSize(int& w, int& h) const
    {
        w = 0;
        h = 0;
        if (window_)
            glfwGetFramebufferSize(window_, &w, &h);

    }

    void WindowGLFW::setTitle(std::string_view title)
    {
        if (window_)
            glfwSetWindowTitle(window_, std::string(title).c_str());

    }

    void* WindowGLFW::nativeHandle() const
    {
        //so we can create a surface
        return window_;
    }
}