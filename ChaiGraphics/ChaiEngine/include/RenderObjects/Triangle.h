#include <RenderObjects/RenderObject.h>
#include <glm/glm.hpp>

namespace CGraphics
{
	class CHAIGRAPHICS_EXPORT TriangleRO : public RenderObject
	{
	public:
		TriangleRO();
		~TriangleRO();

	private:
		SharedVBO<glm::vec3> m_vertexData;
	};
}