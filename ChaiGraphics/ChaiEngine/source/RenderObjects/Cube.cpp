#include <RenderObjects/Cube.h>

namespace CGraphics
{
	CubeRO::CubeRO()
	{
		m_vertexData = createVertexBuffer<glm::vec3>(DataType::FLOAT, 3);
		AddVertexBuffer(m_vertexData, 0);
		glm::vec3 p1 = { 0.5f,  0.5f, 0.0f };
		glm::vec3 p2 = { 0.5f, -0.5f, 0.0f };
		glm::vec3 p3 = { -0.5f, -0.5f, 0.0f };
		glm::vec3 p4 = { -0.5f,  0.5f, 0.0f };
		m_vertexData->data = { p1, p2, p3, p4 };

		m_colorData = createVertexBuffer<glm::vec4>(DataType::FLOAT, 4);
		AddVertexBuffer(m_colorData, 1);
		glm::vec4 c1 = { 1.f,  0.f, 0.f, 1.f };
		glm::vec4 c2 = { 0.f, 1.f, 0.f, 1.f };
		glm::vec4 c3 = { 0.f, 0.f, 1.f, 1.f };
		glm::vec4 c4 = { 1.f,  1.f, 0.f, 1.f };
		m_colorData->data = { c1, c2, c3, c4 };

		m_indexBuffer = createIndexBuffer();
		AddIndexBuffer(m_indexBuffer, 1);
		m_indexBuffer->data = { 0, 1, 3, 1, 2, 3 };


		//shader shit
		const char* vertexShaderSource = "#version 330 core\n"
			"layout (location = 0) in vec3 aPos;\n"
			"layout (location = 1) in vec3 aCol;\n"
			"out vec3 outCol;\n"
			"void main()\n"
			"{\n"
			"   outCol = aCol;\n"
			"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
			"}\0";

		//frag shader shit
		const char* fragmentShaderSource = "#version 330 core\n"
			"in vec3 outCol;\n"  // Matches 'outCol' from vertex shader
			"out vec4 FragColor;\n"
			"void main()\n"
			"{\n"
			"   FragColor = vec4(outCol, 1.0);\n"  // Use color from vertex shader
			"}\0";

		AddShaderSource(vertexShaderSource, ShaderStage::VERTEX);
		AddShaderSource(fragmentShaderSource, ShaderStage::FRAGMENT);
	}

	CubeRO::~CubeRO()
	{

	}
}