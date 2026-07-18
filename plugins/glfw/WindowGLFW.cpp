#include "WindowGLFW.h"
#include <Log.h>
#include "Input.h"

namespace chai
{
    void onFramebufferSize(GLFWwindow* window, int width, int height)
    {
        auto* windowService = static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window));
        windowService->events_.push_back(WindowEvent{WindowEventType::Resized, width, height});
    }

    void onClose(GLFWwindow* window)
    {
        auto* windowService = static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window));
        windowService->events_.push_back(WindowEvent{WindowEventType::CloseRequested});
    }

    void onFocus(GLFWwindow* window, int focused)
    {
        auto* windowService = static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window));
        WindowEvent event;
        event.type =
            focused == GLFW_TRUE ? WindowEventType::FocusGained : WindowEventType::FocusLost;
        windowService->events_.push_back(event);
    }

    void onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        auto* windowService = static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window));
        KeyEvent event;
        if (action == GLFW_PRESS)
            event.type = InputEventType::KeyPress;
        if (action == GLFW_RELEASE)
            event.type = InputEventType::KeyRelease;
        if (action == GLFW_REPEAT)
            event.type = InputEventType::KeyRepeat;
        event.key = toChaiKey(key);
        //TODO: scancode, mods
        windowService->input_->updateKeyPress(event);
    }

    void onMouseButton(GLFWwindow* window, int button, int action, int mods)
    {
        auto* windowService = static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window));
        MouseButtonEvent event;
        event.type = action == GLFW_PRESS ? InputEventType::MouseButtonDown : InputEventType::MouseButtonUp;
        if (button == GLFW_MOUSE_BUTTON_LEFT)
            event.mouse = MouseButton::Left;
        if (button == GLFW_MOUSE_BUTTON_RIGHT)
            event.mouse = MouseButton::Right;
        if (button == GLFW_MOUSE_BUTTON_MIDDLE)
            event.mouse = MouseButton::Middle;

        windowService->input_->updateMousePress(event);
    }

    void onCursorMove(GLFWwindow* window, double xpos, double ypos)
    {
        auto* windowService = static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window));
        MouseMoveEvent event;
        event.type = InputEventType::MouseMove;
        event.x = static_cast<float>(xpos);
        event.y = static_cast<float>(ypos);
        windowService->input_->updateMouseMove(event);

    }

    void onCharInput(GLFWwindow* window, unsigned int codepoint) 
    {
        auto* windowService = static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window));
        CharEvent event;
        event.type = InputEventType::Char;
        event.input = codepoint;
        windowService->input_->updateCharInput(event);
    }

    void OnScroll(GLFWwindow* window, double xoffset, double yoffset)
    {
        auto* windowService = static_cast<WindowGLFW*>(glfwGetWindowUserPointer(window));
        MouseScrollEvent event;
        event.type = InputEventType::Char;
        event.x = xoffset;
        event.y = yoffset;
        windowService->input_->updateScrollInput(event);
    }

    WindowGLFW::WindowGLFW(const WindowDesc& desc, class InputHandler* input) : input_(input)
    {
        // TODO: There might be a more graphics api agnostic way to do this
        // But ive got my heart set on vulkan right now, so this is how I will do it
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, desc.resizable ? GLFW_TRUE : GLFW_FALSE);

        window_ = glfwCreateWindow(desc.width, desc.height, desc.title.c_str(), nullptr, nullptr);
        if (!window_) {
            CHAI_LOG_CRITICAL("Failed to create GLFW window");
            return;
        }

        glfwSetWindowUserPointer(window_, this);
        glfwSetFramebufferSizeCallback(window_, onFramebufferSize);
        glfwSetWindowCloseCallback(window_, onClose);
        glfwSetWindowFocusCallback(window_, onFocus);
        glfwSetKeyCallback(window_, onKeyPress);
        glfwSetMouseButtonCallback(window_, onMouseButton);
        glfwSetCursorPosCallback(window_, onCursorMove);
        glfwSetCharCallback(window_, onCharInput);
        glfwSetScrollCallback(window_, OnScroll);

        events_.reserve(16);

        int fbw = 0, fbh = 0;
        glfwGetFramebufferSize(window_, &fbw, &fbh);
        CHAI_LOG_INFO(
            "Window created: {}x{} screen, {}x{} framebuffer", desc.width, desc.height, fbw, fbh);

    }

    WindowGLFW::~WindowGLFW()
    {
        if (window_)
            glfwDestroyWindow(window_);
    }


    std::span<const WindowEvent> WindowGLFW::pollEvents()
    {
        events_.clear();
        glfwPollEvents();
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
        //so we can create a surface for vulkan
        return window_;
    }

    std::vector<const char*> WindowGLFW::getExtensions() const
    {
        uint32_t extensionCount = 0;
        const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

        return std::vector<const char*>(extensions, extensions + extensionCount);
    }

    void* WindowGLFW::createSurface_Vulkan(void* instance) const
    {
        VkSurfaceKHR surface;

        auto vkInstance = static_cast<VkInstance*>(instance);

        if (!vkInstance)
        {
            CHAI_LOG_CRITICAL("Could not get VkInstance, surface creation failed.");
        }

        if (glfwCreateWindowSurface(*vkInstance, window_, nullptr, &surface) !=
            VK_SUCCESS) {
            CHAI_LOG_CRITICAL("Failed to create window surface.");
        }

        return surface;
    }
}