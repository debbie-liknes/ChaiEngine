#include <RenderObjects/Triangle.h>

namespace CGraphics
{
	TriangleRO::TriangleRO()
	{
		m_vertexData = createVertexBuffer<glm::vec3>();
		AddVertexBuffer(m_vertexData, 0);
		glm::vec3 p1 = { -0.5f, -0.5f, 0.0f };
		glm::vec3 p2 = { 0.5f, -0.5f, 0.0f };
		glm::vec3 p3 = { 0.0f,  0.5f, 0.0f };
		m_vertexData->data = { p1, p2, p3 };


		//shader shit
		const char* vertexShaderSource = "#version 330 core\n"
			"layout (location = 0) in vec3 aPos;\n"
			"void main()\n"
			"{\n"
			"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
			"}\0";

		//frag shader shit
		const char* fragShaderSource = "#version 330 core\n"
			"out vec4 FragColor;\n"
			"void main()\n"
			"{\n"
			"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
			"}\0";

		AddShaderSource(vertexShaderSource, ShaderStage::VERTEX);
		AddShaderSource(fragShaderSource, ShaderStage::FRAGMENT);
	}

	TriangleRO::~TriangleRO()
	{

	}
}