#include <RenderObjects/Triangle.h>

namespace CGraphics
{
	TriangleRO::TriangleRO()
	{
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

		AddShader(vertexShaderSource, ShaderStage::VERTEX);
		AddShader(fragShaderSource, ShaderStage::FRAGMENT);

		m_verts = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f,  0.5f, 0.0f
		};
	}

	TriangleRO::~TriangleRO()
	{

	}
}