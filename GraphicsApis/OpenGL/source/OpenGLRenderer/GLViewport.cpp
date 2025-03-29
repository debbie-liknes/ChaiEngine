#include <OpenGLRenderer/GLViewport.h>
#include <glad/glad.h>

namespace chai::brew
{
	GLViewport::GLViewport()
	{

	}

	void GLViewport::bind()
	{
		//probably need to optimize this
		glViewport(m_x, m_y, m_width, m_height);
	}
}