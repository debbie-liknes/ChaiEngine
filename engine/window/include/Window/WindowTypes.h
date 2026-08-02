/**
 * @file WindowTypes.h
 * @brief Core types for Window Management
 */
#pragma once
#include <string>
#include <memory>
#include <span>

namespace chai
{
    /**
     * @brief Types of Window Events. Resized events relates to the the framebuffer being resized (pixels).
     */
    enum class WindowEventType 
    {
        None = 0,
        Resized,
        CloseRequested,
        FocusGained,
        FocusLost,
    };

    /**
     * @brief Structure for window events.
     * 
     * @note Avoid adding types other than primitives. Keep the Core library clean
     */
    struct WindowEvent 
    {
        WindowEventType type = WindowEventType::None;
        int width = 0;
        int height = 0;
    };

    /**
     * @brief Construction parameters
     */
    struct WindowDesc 
    {
        std::string title = "ChaiEngine";
        int width = 1280; //screen coords
        int height = 720; //screen coords
        bool resizable = true;
    };
}