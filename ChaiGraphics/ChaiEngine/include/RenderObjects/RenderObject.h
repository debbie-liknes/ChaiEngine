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
		std::pair<uint16_t, std::shared_ptr<VertexBufferBase>> m_indexBuffer;


		bool isDirty();
		void setDirty(bool dirty = true);
		bool hasIndexBuffer();
		PrimitiveMode getPrimitiveType();

	protected:
		void AddShader(std::string file, ShaderStage stage);
		void AddShaderSource(std::string dataString, ShaderStage stage);

		void AddVertexBuffer(Core::CSharedPtr<VertexBufferBase> vbo, uint16_t binding);
		void AddIndexBuffer(Core::CSharedPtr<VertexBufferBase> vbo, uint16_t binding);

		PrimitiveMode m_primType = PrimitiveMode::TRIANGLES;

	private:
		bool m_dirty = true;
		bool m_hasIndexBuffer = false;
	};
}