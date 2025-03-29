#pragma once
#include <Renderables/Renderable.h>
#include <Renderables/Cube.h>
#include <memory>

namespace chai::cup
{
	class Entity
	{
	public:
		Entity()
		{
			m_testRenderables.push_back(std::make_shared<chai::brew::CubeRO>());
		}
		//probably going to be a mesh? or similar
		std::vector<std::shared_ptr<brew::Renderable>> m_testRenderables;
	};
}