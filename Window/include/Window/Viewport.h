#pragma once
#include <WindowModuleExport.h>
#include <Core/MemoryTypes.h>

namespace chai
{
    class WINDOWMODULE_EXPORT Viewport
    {
    public:
        Viewport();
        Viewport(int posX, int posY, int width, int height);
        ~Viewport() = default;

        // Accessors
        void setDimensions(int x, int y, int width, int height);
        void getDimensions(int& x, int& y, int& width, int& height) const;

        virtual void bind() = 0;

    protected:
        int m_x, m_y, m_width, m_height;
    };
    typedef std::shared_ptr<Viewport> SharedViewport;
}