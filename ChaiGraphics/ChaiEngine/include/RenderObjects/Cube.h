#include <RenderObjects/RenderObject.h>
#include <glm/glm.hpp>

namespace chai::brew
{
	class CHAIGRAPHICS_EXPORT CubeRO : public RenderObject
	{
	public:
		CubeRO();
		~CubeRO();

	private:
		SharedVBO<glm::vec3> m_vertexData;
		SharedVBO<glm::vec3> m_normalData;
		SharedUBO<glm::vec3> m_colorData;
		SharedVBO<uint32_t> m_indexBuffer;
	};
}