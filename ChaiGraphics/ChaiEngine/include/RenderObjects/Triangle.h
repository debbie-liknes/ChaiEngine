#include <RenderObjects/RenderObject.h>
#include <glm/glm.hpp>

namespace chai_graphics
{
	class CHAIGRAPHICS_EXPORT TriangleRO : public RenderObject
	{
	public:
		TriangleRO();
		~TriangleRO();

	private:
		SharedVBO<glm::vec3> m_vertexData;
		SharedVBO<glm::vec4> m_colorData;
		SharedVBO<uint32_t> m_indexBuffer;
	};
}