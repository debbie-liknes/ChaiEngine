#include <Renderables/Triangle.h>

namespace chai::brew
{
	TriangleRO::TriangleRO()
	{
		m_vertexData = createVertexBuffer<glm::vec3>(PrimDataType::FLOAT, 3);
		AddVertexBuffer(m_vertexData, 0);
		glm::vec3 p1 = { 0.5f,  0.5f, 0.0f };
		glm::vec3 p2 = { 0.5f, -0.5f, 0.0f };
		glm::vec3 p3 = { -0.5f, -0.5f, 0.0f };
		glm::vec3 p4 = { -0.5f,  0.5f, 0.0f };
		m_vertexData->data = { p1, p2, p3, p4 };

		m_colorData = createVertexBuffer<glm::vec4>(PrimDataType::FLOAT, 4);
		AddVertexBuffer(m_colorData, 1);
		glm::vec4 c1 = { 1.f,  0.f, 0.f, 1.f };
		glm::vec4 c2 = { 0.f, 1.f, 0.f, 1.f };
		glm::vec4 c3 = { 0.f, 0.f, 1.f, 1.f };
		glm::vec4 c4 = { 1.f,  1.f, 0.f, 1.f };
		m_colorData->data = { c1, c2, c3, c4 };

		m_indexBuffer = createIndexBuffer();
		AddIndexBuffer(m_indexBuffer, 1);
		m_indexBuffer->data = { 0, 1, 3, 1, 2, 3 };

		AddShader("Data/Shaders/shape.vert", ShaderStage::VERTEX);
		AddShader("Data/Shaders/shape.frag", ShaderStage::FRAGMENT);
	}

	TriangleRO::~TriangleRO()
	{

	}
}