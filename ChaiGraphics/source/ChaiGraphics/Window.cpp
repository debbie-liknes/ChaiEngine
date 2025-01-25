#include <ChaiGraphics/Window.h>
#include <ChaiGraphics/Viewport.h>

namespace CGraphics
{
	Window::Window(Core::CString title, int width, int height) : m_title(title), m_width(width), m_height(height)
	{
	}

	void Window::AddViewport(SharedViewport viewport)
	{
		m_viewports.push_back(viewport);
	}

	void Window::RemoveViewport(SharedViewport viewport)
	{
		auto port = std::find(m_viewports.begin(), m_viewports.end(), viewport);
		if (port != m_viewports.end())
		{
			m_viewports.erase(port);
		}
	}

	int Window::GetHeight() const
	{
		return m_height;
	}

	int Window::GetWidth() const
	{
		return m_width;
	}

	Core::CString Window::GetTitle() const
	{
		return m_title;
	}
}