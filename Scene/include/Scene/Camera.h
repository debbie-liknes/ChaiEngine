#include <SceneExport.h>
#include <glm/glm.hpp>

namespace chai_scene
{
	class SCENE_EXPORT Camera
	{
	public:
		Camera();
		virtual ~Camera();

	//private:
		glm::vec3 m_position;
		glm::mat4 m_projMat;
		glm::mat4 m_viewMat;
	};
}