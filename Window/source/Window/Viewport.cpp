#include <Window/Viewport.h>
//#include <>


namespace chai
{
	Viewport::Viewport()
	{

	}

	Viewport::Viewport(int posX, int posY, int width, int height)
		: m_x(posX), m_y(posY), m_width(width), m_height(height)
	{

	}

	void Viewport::setDimensions(int x, int y, int width, int height)
	{
		m_x = x;
		m_y = y;
		m_width = width;
		m_height = height;
	}

	void Viewport::getDimensions(int& x, int& y, int& width, int& height) const
	{
		x = m_x;
		y = m_y;
		width = m_width;
		height = m_height;
	}
}