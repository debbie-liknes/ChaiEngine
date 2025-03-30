#include <Renderables/Cube.h>

namespace chai::brew
{
	CubeRO::CubeRO()
	{
		//unshared vertices, for flat shading
		m_vertexData = createVertexBuffer<glm::vec3>(PrimDataType::FLOAT, 3);
		AddVertexBuffer(m_vertexData, 0);
		glm::vec3 p0 = { 0.5f,  0.5f, -0.5f };
		glm::vec3 p1 = { 0.5f, -0.5f, -0.5f };
		glm::vec3 p2 = { -0.5f, -0.5f, -0.5f };
		glm::vec3 p3 = { -0.5f,  0.5f, -0.5f };

		glm::vec3 p4 = { 0.5f,  0.5f, 0.5f };
		glm::vec3 p5 = { 0.5f, -0.5f, 0.5f };
		glm::vec3 p6 = { -0.5f, -0.5f, 0.5f };
		glm::vec3 p7 = { -0.5f,  0.5f, 0.5f };
		m_vertexData->data = { 
			p4, p5, p6, p7, //facing +z
			p0, p1, p2, p3, //facing -z
			p0, p3, p4, p7,	//facing +y
			p1, p2, p5, p6,	//facing -y
			p0, p1, p4, p5,	//facing +x
			p2, p3, p6, p7	//facing -x
		};

		//normals
		m_normalData = createVertexBuffer<glm::vec3>(PrimDataType::FLOAT, 3);
		AddVertexBuffer(m_vertexData, 1);
		glm::vec3 n0 = { 0.f,  0.f, 1.f };
		glm::vec3 n1 = { 0.f, 0.f, -1.f };
		glm::vec3 n2 = { 0.f, 1.f, 0.f };
		glm::vec3 n3 = { 0.f,  -1.f, 0.f };
		glm::vec3 n4 = { 1.f,  0.f, 0.f };
		glm::vec3 n5 = { -1.f, 0.f, 0.f };

		m_normalData->data = { 
			n0, n0, n0, n0, 
			n1, n1, n1, n1, 
			n2, n2, n2, n2, 
			n3, n3, n3, n3, 
			n4, n4, n4, n4, 
			n5, n5, n5, n5
		};

		m_colorData = createUniformBuffer<glm::vec3>(PrimDataType::FLOAT);
		AddUniform(m_colorData, 1);
		m_colorData->data = { 0.f, 1.f, 0.f };
		m_colorData->name = "ColorData";

		m_indexBuffer = createIndexBuffer();
		AddIndexBuffer(m_indexBuffer, 1);
		m_indexBuffer->data = { 
			1, 0, 3, 2, 1, 3,
			6, 7, 4, 4, 5, 6, 
			8, 9, 10, 10, 9, 11,
			13, 12, 14, 13, 14, 15,
			17, 16, 18, 17, 18, 19,
			21, 20, 22, 21, 22, 23
		};

		RequestViewData();

		AddShader("Data/Shaders/shape.vert", ShaderStage::VERTEX);
		AddShader("Data/Shaders/shape.frag", ShaderStage::FRAGMENT);
	}

	CubeRO::~CubeRO()
	{

	}
}