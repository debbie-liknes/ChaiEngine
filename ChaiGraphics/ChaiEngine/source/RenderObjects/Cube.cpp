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

		glm::vec3 p5 = { 0.5f,  0.5f, 0.5f };
		glm::vec3 p6 = { 0.5f, -0.5f, 0.5f };
		glm::vec3 p7 = { -0.5f, -0.5f, 0.5f };
		glm::vec3 p8 = { -0.5f,  0.5f, 0.5f };
		m_vertexData->data = { p1, p2, p3, p4, p5, p6, p7, p8 };

		//normals
		m_normalData = createVertexBuffer<glm::vec3>(PrimDataType::FLOAT, 3);
		AddVertexBuffer(m_vertexData, 1);
		glm::vec3 n1 = { 0.f,  1.f, 0.0f };
		glm::vec3 n2 = { 0.f, -1.f, 0.0f };
		glm::vec3 n3 = { 0.f, -1.f, 0.0f };
		glm::vec3 n4 = { 0.f,  1.f, 0.0f };

		glm::vec3 n5 = { 0.f,  1.f, 0.0f };
		glm::vec3 n6 = { 0.f, -1.f, 0.0f };
		glm::vec3 n7 = { 0.f, -1.f, 0.0f };
		glm::vec3 n8 = { 0.f,  1.f, 0.0f };
		m_normalData->data = { n1, n2, n3, n4, n5, n6, n7, n8 };

		//top face: 0, 3, 4, 7

		m_colorData = createUniformBuffer<glm::vec3>(PrimDataType::FLOAT);
		AddUniform(m_colorData, 1);
		m_colorData->data = { 0.f, 1.f, 0.f };
		m_colorData->name = "ColorData";

		m_indexBuffer = createIndexBuffer();
		AddIndexBuffer(m_indexBuffer, 1);
		m_indexBuffer->data = { 
			0, 1, 3, 1, 2, 3, //front face
			8, 5, 6, 7, 6, 8, 
			0, 3, 4, 3, 4, 7
		};

		RequestViewData();


		//shader shit
		const char* vertexShaderSource = "#version 330 core\n"
			"layout (location = 0) in vec3 aPos;\n"
			"layout (location = 1) in vec3 aNorm;\n"
			"layout (std140) uniform MatrixData {\n"
			"mat4 proj;\n"
			"mat4 view;\n"
			"} ubo;\n"
			"layout (std140) uniform ModelData {\n"
			"mat4 ma;\n"
			"} model;\n"
			"out vec3 outNorm;\n"
			"out vec3 FragPos;\n"
			"void main()\n"
			"{\n"
			"   outNorm = aNorm;\n"
			"   FragPos = vec3(model.ma * vec4(aPos, 1.0));\n"
			"   gl_Position = ubo.proj * ubo.view * model.ma * vec4(aPos, 1.0);\n"
			"}\0";

		//frag shader shit
		const char* fragmentShaderSource = "#version 330 core\n"
			"in vec3 outNorm;\n"  // Matches 'outNorm' from vertex shader
			"in vec3 FragPos;\n"  // Matches 'outNorm' from vertex shader
			"out vec4 FragColor;\n"
			"layout (std140) uniform ColorData {\n"
			"vec3 color;\n"
			"} colorUBO;\n"
			"void main()\n"
			"{\n"
			"   vec3 lightPos = vec3(10.f, 10.f, 10.f);\n"  // Use color from vertex shader
			"   vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);\n"  // Use color from vertex shader
			"   vec3 objColor = vec3(1.0f, 0.5f, 0.31f);\n"  // Use color from vertex shader
			"   vec3 viewPosition = vec3(0.0, 0.0, 5.0);\n"  // Use color from vertex shader
			"   float ambientStrength = 0.1;\n"  // Use color from vertex shader
			"   vec3 ambient = ambientStrength * lightColor;\n"  // Use color from vertex shader
			//"   vec3 result = ambient * objColor;\n"  // Use color from vertex shader
			"   vec3 lightDir = normalize(lightPos - FragPos);\n"  // Use color from vertex shader
			"   float diff = max(dot(outNorm, lightDir), 0.0);\n"  // Use color from vertex shader
			"   vec3 diffuse = diff * lightColor;\n"  // Use color from vertex shader
			"   vec3 result = (ambient + diffuse) * objColor;\n"  // Use color from vertex shader
			"   FragColor = vec4(result, 1.0);\n"  // Use color from vertex shader
			"}\0";

		AddShaderSource(vertexShaderSource, ShaderStage::VERTEX);
		AddShaderSource(fragmentShaderSource, ShaderStage::FRAGMENT);
	}

	CubeRO::~CubeRO()
	{

	}
}