/**
 * @file WindowTypes.h
 * @brief Core types for Window Management
 */
#pragma once
#include <CoreExport.h>
#include <string>
#include <memory>
#include <span>

namespace chai
{
    /**
     * @brief Types of Window Events. Resized events relates to the the framebuffer being resized (pixels).
     */
    enum class CORE_EXPORT WindowEventType 
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
    struct CORE_EXPORT WindowEvent 
    {
        WindowEventType type = WindowEventType::None;
        int width = 0;
        int height = 0;
    };

    /**
     * @brief Construction parameters
     */
    struct CORE_EXPORT WindowDesc 
    {
        std::string title = "ChaiEngine";
        int width = 1280; //screen coords
        int height = 720; //screen coords
        bool resizable = true;
    };
}