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
}