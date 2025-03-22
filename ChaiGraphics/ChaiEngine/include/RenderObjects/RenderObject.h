#pragma once
#include <ChaiGraphicsExport.h>
#include <Core/Containers.h>
#include <Core/MemoryTypes.h>
#include <Engine/VertexBuffer.h>
#include <Engine/UniformBuffer.h>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace CGraphics
{
	enum CHAIGRAPHICS_EXPORT ShaderStage
	{
		VERTEX,
		FRAGMENT,
		NONE
	};

	struct CHAIGRAPHICS_EXPORT ShaderData
	{
		Core::CString shaderSource = "";
		ShaderStage stage = ShaderStage::NONE;
	};

	enum PrimitiveMode
	{
		POINTS,
		LINES,
		TRIANGLES
	};

	class CHAIGRAPHICS_EXPORT RenderObject
	{
	public:
		RenderObject();
		~RenderObject();

		Core::CVector<ShaderData> m_data;
		std::map<uint16_t, std::shared_ptr<VertexBufferBase>> m_vertexBuffers;
		std::map<uint16_t, std::shared_ptr<UniformBufferBase>> m_uniforms;
		std::pair<uint16_t, std::shared_ptr<VertexBufferBase>> m_indexBuffer;


		bool isDirty();
		void setDirty(bool dirty = true);
		bool hasIndexBuffer();
		PrimitiveMode getPrimitiveType();

		glm::mat4 getModelMatrix() const;

	protected:
		void AddShader(std::string file, ShaderStage stage);
		void AddShaderSource(std::string dataString, ShaderStage stage);

		void AddVertexBuffer(Core::CSharedPtr<VertexBufferBase> vbo, uint16_t binding);
		void AddIndexBuffer(Core::CSharedPtr<VertexBufferBase> vbo, uint16_t binding);
		void AddUniform(Core::CSharedPtr<UniformBufferBase> ubo, uint16_t binding);

		PrimitiveMode m_primType = PrimitiveMode::TRIANGLES;

	private:
		bool m_dirty = true;
		bool m_hasIndexBuffer = false;

		glm::vec3 m_position;
		glm::quat m_rotation;
		glm::mat4 m_modelMat;
		glm::vec3 m_scale;
	};
}