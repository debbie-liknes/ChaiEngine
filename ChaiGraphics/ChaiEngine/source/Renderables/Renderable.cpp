#include <Renderables/Renderable.h>

namespace chai::brew
{
	Renderable::Renderable() : m_position(glm::vec3(0.f)), m_rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)), m_scale(glm::vec3(1.f))
	{
		m_modelMat = createUniformBuffer<glm::mat4>(PrimDataType::FLOAT);
		AddUniform(m_modelMat, 0);
		m_modelMat->data = getModelMatrix();
		m_modelMat->name = "ModelData";
	}

	Renderable::~Renderable()
	{

	}

	void Renderable::AddShader(std::string file, ShaderStage stage)
	{
		m_data.push_back({ file, stage });
	}

	bool Renderable::isDirty()
	{
		return m_dirty;
	}

	void Renderable::setDirty(bool dirty)
	{
		m_dirty = dirty;
	}

	bool Renderable::hasIndexBuffer()
	{
		return m_hasIndexBuffer;
	}

	void Renderable::AddVertexBuffer(chai::CSharedPtr<VertexBufferBase> vbo, uint16_t binding)
	{
		m_vertexBuffers[binding] = vbo;
	}

	void Renderable::AddIndexBuffer(chai::CSharedPtr<VertexBufferBase> vbo, uint16_t binding)
	{
		m_indexBuffer = { binding, vbo };
		m_hasIndexBuffer = true;
	}

	void Renderable::AddUniform(chai::CSharedPtr<UniformBufferBase> ubo, uint16_t binding)
	{
		m_uniforms[binding] = ubo;
	}

	PrimitiveMode Renderable::getPrimitiveType()
	{
		return m_primType;
	}

	glm::mat4 Renderable::getModelMatrix() const
	{
		return glm::translate(glm::mat4(1.f), m_position) * glm::mat4_cast(m_rotation) * glm::scale(glm::mat4(1.f), m_scale);
	}

	void Renderable::RequestViewData(bool req)
	{
		m_addViewData = req;
	}

	glm::vec3 Renderable::getPosition() const
	{
		return m_position;
	}

	void Renderable::setPosition(glm::vec3 pos)
	{
		m_position = pos;
	}
}