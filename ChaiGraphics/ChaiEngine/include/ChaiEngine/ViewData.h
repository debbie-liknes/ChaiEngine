#pragma once
#include <ChaiGraphicsExport.h>
#include <glm/glm.hpp>

namespace chai_graphics
{
	struct CHAIGRAPHICS_EXPORT ViewData
	{
		glm::mat4 projMat;
		glm::mat4 view;
	};
}