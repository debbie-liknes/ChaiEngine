#include <RenderObjects/RenderObject.h>

namespace chai_graphics
{
	RenderObject::RenderObject() : m_position(glm::vec3(0.f)), m_rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)), m_scale(glm::vec3(1.f))
	{
		m_modelMat = createUniformBuffer<glm::mat4>(PrimDataType::FLOAT);
		AddUniform(m_modelMat, 0);
		m_modelMat->data = getModelMatrix();
		m_modelMat->name = "ModelData";
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

	void RenderObject::AddUniform(Core::CSharedPtr<UniformBufferBase> ubo, uint16_t binding)
	{
		m_uniforms[binding] = ubo;
	}

	PrimitiveMode RenderObject::getPrimitiveType()
	{
		return m_primType;
	}

	glm::mat4 RenderObject::getModelMatrix() const
	{
		return glm::translate(glm::mat4(1.f), m_position) * glm::mat4_cast(m_rotation) * glm::scale(glm::mat4(1.f), m_scale);
	}

	void RenderObject::RequestViewData(bool req)
	{
		m_addViewData = req;
	}
}