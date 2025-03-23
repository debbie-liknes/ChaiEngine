#include <Renderables/Renderable.h>
#include <glm/glm.hpp>

namespace chai::brew
{
	class CHAIGRAPHICS_EXPORT TriangleRO : public Renderable
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