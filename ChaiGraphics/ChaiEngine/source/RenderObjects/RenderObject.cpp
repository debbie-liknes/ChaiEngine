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
		m_data.push_back({ dataString, stage });
	}
}