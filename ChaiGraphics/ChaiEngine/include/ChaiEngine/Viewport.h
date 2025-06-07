#pragma once
#include <Core/MemoryTypes.h>

namespace chai
{
    class Viewport
    {
    public:
        Viewport();
        Viewport(int posX, int posY, int width, int height);
        ~Viewport() = default;

        float minDepth = 0.0f, maxDepth = 1.0f;
        int x, y, width, height;
    };
    typedef std::shared_ptr<Viewport> SharedViewport;
}