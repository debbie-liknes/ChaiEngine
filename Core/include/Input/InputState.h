#pragma once
#include <array>

namespace chai
{
    struct InputState
    {
        std::array<bool, 256> keys = {false};
        float mouseX = 0.0f;
        float mouseY = 0.0f;
        float lastMouseX = 0.0f;
        float lastMouseY = 0.0f;
        bool firstMouse = true;
        bool mouseCaptured = false;
    };
}