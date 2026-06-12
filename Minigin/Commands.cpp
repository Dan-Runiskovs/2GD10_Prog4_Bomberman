#include "Commands.h"
#include "GameObject.h"
#include "PhysicsComponent.h"

dae::GameObjectCommand::GameObjectCommand(GameObject& object) noexcept
	:m_Object{ object }
{
}

dae::MoveCommand::MoveCommand(GameObject& object, int8_t x, int8_t y, float searchRadius) noexcept
	: GameObjectCommand(object)
	, m_Movement{ object.GetComponent<PhysicsComponent>() }
	, m_DirX{ x }
	, m_DirY{ y }
	, m_SearchRadius{ searchRadius }
{
}

void dae::MoveCommand::Execute()
{
	m_Movement.TryMove(
		glm::vec2(
			static_cast<float>(m_DirX),
			static_cast<float>(m_DirY)
		), m_SearchRadius);
}


