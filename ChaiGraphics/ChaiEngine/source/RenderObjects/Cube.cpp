#include <RenderObjects/Cube.h>

namespace chai::brew
{
	CubeRO::CubeRO()
	{
		m_vertexData = createVertexBuffer<glm::vec3>(PrimDataType::FLOAT, 3);
		AddVertexBuffer(m_vertexData, 0);
		glm::vec3 p1 = { 0.5f,  0.5f, 0.0f };
		glm::vec3 p2 = { 0.5f, -0.5f, 0.0f };
		glm::vec3 p3 = { -0.5f, -0.5f, 0.0f };
		glm::vec3 p4 = { -0.5f,  0.5f, 0.0f };
		m_vertexData->data = { p1, p2, p3, p4 };

		m_colorData = createUniformBuffer<glm::vec3>(PrimDataType::FLOAT);
		AddUniform(m_colorData, 1);
		m_colorData->data = { 0.f, 1.f, 0.f };
		m_colorData->name = "ColorData";

		m_indexBuffer = createIndexBuffer();
		AddIndexBuffer(m_indexBuffer, 1);
		m_indexBuffer->data = { 0, 1, 3, 1, 2, 3 };

		RequestViewData();


		//shader shit
		const char* vertexShaderSource = "#version 330 core\n"
			"layout (location = 0) in vec3 aPos;\n"
			"layout (std140) uniform MatrixData {\n"
			"mat4 proj;\n"
			"mat4 view;\n"
			"} ubo;\n"
			"layout (std140) uniform ModelData {\n"
			"mat4 ma;\n"
			"} model;\n"
			"void main()\n"
			"{\n"
			"   gl_Position = ubo.proj * ubo.view * model.ma * vec4(aPos, 1.0);\n"
			"}\0";

		//frag shader shit
		const char* fragmentShaderSource = "#version 330 core\n"
			"in vec3 outCol;\n"  // Matches 'outCol' from vertex shader
			"out vec4 FragColor;\n"
			"layout (std140) uniform ColorData {\n"
			"vec3 color;\n"
			"} colorUBO;\n"
			"void main()\n"
			"{\n"
			"   FragColor = vec4(colorUBO.color, 1.0);\n"  // Use color from vertex shader
			"}\0";

		AddShaderSource(vertexShaderSource, ShaderStage::VERTEX);
		AddShaderSource(fragmentShaderSource, ShaderStage::FRAGMENT);
	}

	CubeRO::~CubeRO()
	{

	}
}