#pragma once

namespace chai::brew
{
    struct Vertex {
        float position[3];
        float normal[3];
        float texCoord[2];
        float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

        Vertex(float px, float py, float pz,
            float nx = 0.0f, float ny = 0.0f, float nz = 1.0f,
            float u = 0.0f, float v = 0.0f)
            : position{ px, py, pz }, normal{ nx, ny, nz }, texCoord{ u, v } {
        }
    };
}