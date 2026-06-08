/**
 * @file WindowGLFW.h
 * @brief Concrete implementation of an IWindow
 */
#pragma once
#include <Window/Window.h>
#include <Window/WindowTypes.h>
#include <Plugin/PluginBase.h>
#include <GLFW/glfw3.h>
#include <vector>

namespace chai
{
    class WindowGLFW : public IWindow
    {
    public:
        WindowGLFW(const WindowDesc& desc);
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

    private:
        GLFWwindow* window_;
        std::vector<WindowEvent> events_;

        //void onFramebufferSize(GLFWwindow* window, int width, int height);
    };
}