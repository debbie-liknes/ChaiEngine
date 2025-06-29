#include <Scene/ControllerComponent.h>
#include <Scene/GameObject.h>

namespace chai::cup
{
	ControllerComponent::ControllerComponent(chai::cup::GameObject* owner) : m_owner(owner)
	{
	}
}