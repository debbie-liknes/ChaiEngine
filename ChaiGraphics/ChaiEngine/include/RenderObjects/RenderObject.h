#pragma once
#include <ChaiGraphicsExport.h>
#include <Core/Containers.h>
#include <Core/MemoryTypes.h>
#include <ChaiEngine/VertexBuffer.h>
#include <ChaiEngine/UniformBuffer.h>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ChaiEngine/Shader.h>

namespace chai_graphics
{
	class CHAIGRAPHICS_EXPORT RenderObject
	{
	public:
		RenderObject();
		~RenderObject();

		chai::CVector<ShaderData> m_data;
		std::map<uint16_t, std::shared_ptr<VertexBufferBase>> m_vertexBuffers;
		std::map<uint16_t, std::shared_ptr<UniformBufferBase>> m_uniforms;
		std::pair<uint16_t, std::shared_ptr<VertexBufferBase>> m_indexBuffer;


		bool isDirty();
		void setDirty(bool dirty = true);
		bool hasIndexBuffer();
		PrimitiveMode getPrimitiveType();

		glm::mat4 getModelMatrix() const;

		bool m_addViewData = false;
	protected:
		void AddShader(std::string file, ShaderStage stage);
		void AddShaderSource(std::string dataString, ShaderStage stage);

		void AddVertexBuffer(chai::CSharedPtr<VertexBufferBase> vbo, uint16_t binding);
		void AddIndexBuffer(chai::CSharedPtr<VertexBufferBase> vbo, uint16_t binding);
		void AddUniform(chai::CSharedPtr<UniformBufferBase> ubo, uint16_t binding);
		void RequestViewData(bool req = true);

		PrimitiveMode m_primType = PrimitiveMode::TRIANGLES;

	private:
		bool m_dirty = true;
		bool m_hasIndexBuffer = false;

		glm::vec3 m_position;
		glm::quat m_rotation;
		glm::vec3 m_scale;

		SharedUBO<glm::mat4> m_modelMat;
	};
}