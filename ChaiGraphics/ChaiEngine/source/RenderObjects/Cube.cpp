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

		AddShader("Data/Shaders/shape.vert", ShaderStage::VERTEX);
		AddShader("Data/Shaders/shape.frag", ShaderStage::FRAGMENT);
	}

	CubeRO::~CubeRO()
	{

	}
}