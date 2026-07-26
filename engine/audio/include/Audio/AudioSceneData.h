#pragma once

#include <ChaiMath.h>

#include <vector>

namespace chai::audio
{
	struct AudioListener
	{
        int id;
		math::Vec3 pos;
        math::Vec3 lookAt;
        math::Vec3 up;
	};

	struct AudioSceneData
	{
        std::vector<AudioListener> listeners;
	};
}
