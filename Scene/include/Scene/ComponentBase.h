#pragma once
#include <SceneExport.h>

namespace chai::cup
{
	class GameObject;
	class SCENE_EXPORT Component
	{
	public:
		Component(GameObject* owningObject = nullptr) : m_owningObject(owningObject) {}
		virtual ~Component() = default;
		virtual void update() {}

		GameObject* getGameObject() const { return m_owningObject; }

	private:
		GameObject* m_owningObject = nullptr;
	};
}