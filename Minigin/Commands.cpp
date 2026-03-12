#include "Commands.h"
#include "GameObject.h"
#include "PhysicsComponent.h"

dae::GameObjectCommand::GameObjectCommand(GameObject& object) noexcept
	:m_Object{ object }
{
}

dae::MoveCommand::MoveCommand(GameObject& object, int8_t x, int8_t y) noexcept
	: GameObjectCommand(object)
	, m_Movement{ object.GetComponent<PhysicsComponent>() }
	, m_DirX{ x }
	, m_DirY{ y }
{
}

void dae::MoveCommand::Execute()
{
	m_Movement.TryMove(m_DirX, m_DirY);
}
