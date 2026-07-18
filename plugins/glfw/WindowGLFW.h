/**
 * @file WindowGLFW.h
 * @brief Concrete implementation of an IWindow
 */
#pragma once
#include <Window/Window.h>
#include <Window/WindowTypes.h>
#include <Plugin/PluginBase.h>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace chai
{
    class WindowGLFW : public IWindow
    {
    public:
        WindowGLFW(const WindowDesc& desc, class InputHandler* input);
        ~WindowGLFW() override;

        /**
         * @brief Per frame event pump
         */
        virtual std::span<const WindowEvent> pollEvents() override;

        /**
         * @brief Query the close state
         */
        virtual bool shouldClose() const override;

        /**
         * @brief Request to close the window.
         * Will be processed when safe
         */
        virtual void requestClose() override;

        /**
         * @brief Screen coordinates
         */
        virtual void windowSize(int& w, int& h) const override;

        /**
         * @brief Pixels
         */
        virtual void framebufferSize(int& w, int& h) const override;

        /**
         * @brief Window title setter
         */
        virtual void setTitle(std::string_view title) override;

        /**
         * @brief Opaque
         */
        virtual void* nativeHandle() const override;

        /**
         * @brief Get the extensions needed for the graphics api
         */
        std::vector<const char*> getExtensions() const override;

        /**
         * @brief Create surface for Vulkan
         */
        void* createSurface_Vulkan(void* instance) const override;

    private:
        GLFWwindow* window_;
        std::vector<WindowEvent> events_;
        InputHandler* input_;

        friend void onFramebufferSize(GLFWwindow* window, int width, int height);
        friend void onClose(GLFWwindow* window);
        friend void onFocus(GLFWwindow* window, int focused);
        friend void onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods);
        friend void onMouseButton(GLFWwindow* window, int button, int action, int mods);
        friend void onCursorMove(GLFWwindow* window, double xpos, double ypos);
        friend void onCharInput(GLFWwindow* window, unsigned int codepoint);
        friend void OnScroll(GLFWwindow* window, double xoffset, double yoffset);

    };
}