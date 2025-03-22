#include <RenderObjects/RenderObject.h>

namespace CGraphics
{
	RenderObject::RenderObject()
	{
	}

	RenderObject::~RenderObject()
	{

	}

	void RenderObject::AddShader(std::string dataString, ShaderStage stage)
	{

	}

	void RenderObject::AddShaderSource(std::string dataString, ShaderStage stage)
	{
		m_data.push_back({ dataString, stage });
	}

	bool RenderObject::isDirty()
	{
		return m_dirty;
	}

	void RenderObject::setDirty(bool dirty)
	{
		m_dirty = dirty;
	}

	bool RenderObject::hasIndexBuffer()
	{
		return m_hasIndexBuffer;
	}

	void RenderObject::AddVertexBuffer(Core::CSharedPtr<VertexBufferBase> vbo, uint16_t binding)
	{
		m_vertexBuffers[binding] = vbo;
	}

	void RenderObject::AddIndexBuffer(Core::CSharedPtr<VertexBufferBase> vbo, uint16_t binding)
	{
		m_indexBuffer = { binding, vbo };
		m_hasIndexBuffer = true;
	}

	PrimitiveMode RenderObject::getPrimitiveType()
	{
		return m_primType;
	}

	glm::mat4 RenderObject::getModelMatrix() const
	{
		return glm::translate(glm::mat4(1.f), m_position) * glm::mat4_cast(m_rotation) * glm::scale(glm::mat4(1.f), m_scale);
	}
}