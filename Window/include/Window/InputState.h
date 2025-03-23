#include <WindowModuleExport.h>
#include <glm/glm.hpp>
#include <set>

namespace chai
{
	enum class Key
	{
		KEY_A = 65,
		KEY_B,
		KEY_C,
		KEY_D,
		KEY_E,
		KEY_F,
		KEY_G,
		KEY_H,
		KEY_I,
		KEY_J,
		KEY_K,
		KEY_L,
		KEY_M,
		KEY_N,
		KEY_O,
		KEY_P,
		KEY_Q,
		KEY_R,
		KEY_S,
		KEY_T,
		KEY_U,
		KEY_V,
		KEY_W,
		KEY_X,
		KEY_Y,
		KEY_Z
	};

	struct WINDOWMODULE_EXPORT KeyEvent
	{
		Key pressed;
	};

	struct WINDOWMODULE_EXPORT InputState
	{
		virtual ~InputState() = default;

		glm::vec2 mousePos;
		std::set<Key> keys;
	};
}