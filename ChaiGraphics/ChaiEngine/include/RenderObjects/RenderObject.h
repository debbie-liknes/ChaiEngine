#pragma once
#include <ChaiGraphicsExport.h>
#include <Core/Containers.h>
#include <Core/MemoryTypes.h>
#include <Engine/VertexBuffer.h>
#include <map>

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

	class CHAIGRAPHICS_EXPORT RenderObject
	{
	public:
		RenderObject();
		~RenderObject();

		Core::CVector<ShaderData> m_data;
		std::map<uint16_t, std::shared_ptr<VertexBufferBase>> m_vertexBuffers;

		bool isDirty();
		void setDirty(bool dirty = true);

	protected:
		void AddShader(std::string file, ShaderStage stage);
		void AddShaderSource(std::string dataString, ShaderStage stage);

		void AddVertexBuffer(Core::CSharedPtr<VertexBufferBase> vbo, uint16_t binding)
		{
			m_vertexBuffers[binding] = vbo;
		}

	private:
		bool m_dirty = true;
	};
}