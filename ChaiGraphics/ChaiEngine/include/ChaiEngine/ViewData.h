#pragma once
#include <ChaiGraphicsExport.h>
#include <glm/glm.hpp>

namespace chai::brew
{
	struct CHAIGRAPHICS_EXPORT ViewData
	{
		glm::mat4 projMat;
		glm::mat4 view;
	};
}