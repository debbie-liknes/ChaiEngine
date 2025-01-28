#include <Engine/Viewport.h>


namespace CGraphics
{
	Viewport::Viewport(int posX, int posY, int width, int height)
		: m_x(posX), m_y(posY), m_width(width), m_height(height)
	{

	}

	void Viewport::SetViewer(SharedViewer viewer)
	{
		m_viewer = viewer;
	}

	SharedViewer Viewport::GetViewer() const
	{
		return m_viewer;
	}

	void Viewport::SetDimensions(int x, int y, int width, int height)
	{
		m_x = x;
		m_y = y;
		m_width = width;
		m_height = height;
	}

	void Viewport::GetDimensions(int& x, int& y, int& width, int& height) const
	{
		x = m_x;
		y = m_y;
		width = m_width;
		height = m_height;
	}
}