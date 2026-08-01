/**
 * @file Window.h
 * @brief Interface for a generic window (glfw, sdl, etc)
 */
#pragma once
#include <string>
#include <memory>
#include <span>
#include <Window/WindowTypes.h>
#include <vector>

namespace chai
{
    class IWindow
    {
    public:
        virtual ~IWindow() = default;

        /**
         * @brief Per frame event pump
         */
        virtual std::span<const WindowEvent> pollEvents() = 0;

        /**
         * @brief Query the close state
         */
        virtual bool shouldClose() const = 0;

        /**
         * @brief Request to close the window.
         * Will be processed when safe
         */
        virtual void requestClose() = 0;

        /**
         * @brief Screen coordinates
         */
        virtual void windowSize(int& w, int& h) const = 0;

        /**
         * @brief Pixels
         */
        virtual void framebufferSize(int& w, int& h) const = 0;

        /**
         * @brief Window title setter
         */
        virtual void setTitle(std::string_view title) = 0;

        /**
         * @brief Opaque
         */
        virtual void* nativeHandle() const = 0;

        /**
         * @brief Get the extensions required
         */
        virtual std::vector<const char*> getExtensions() const = 0;

        /**
         * @brief Create surface for Vulkan
         * @param instance VkInstance object
         */
        virtual void* createSurface_Vulkan(void* instance) const = 0;
    };
}