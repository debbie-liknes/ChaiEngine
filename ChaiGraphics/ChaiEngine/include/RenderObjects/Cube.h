#include <RenderObjects/RenderObject.h>
#include <glm/glm.hpp>

namespace CGraphics
{
	class CHAIGRAPHICS_EXPORT CubeRO : public RenderObject
	{
	public:
		CubeRO();
		~CubeRO();

	private:
		SharedVBO<glm::vec3> m_vertexData;
		SharedUBO<glm::vec3> m_colorData;
		SharedVBO<uint32_t> m_indexBuffer;
	};
}