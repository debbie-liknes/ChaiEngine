#include <Window/Viewport.h>

namespace chai
{
	Viewport::Viewport(uint64_t viewId, const ViewportDesc& desc, uint64_t window) : m_parentWindow(window), m_desc(desc)
    {

    }

    void Viewport::setActive()
    {

    }

    void Viewport::clear()
    {

    }

    void Viewport::setRect(int x, int y, int width, int height)
    {
        m_desc.x = x;
        m_desc.y = y;
        m_desc.width = width;
        m_desc.height = height;
    }

    void Viewport::setClearColor(float r, float g, float b, float a)
    {

    }
}