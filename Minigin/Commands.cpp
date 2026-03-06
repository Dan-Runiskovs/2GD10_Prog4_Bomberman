#include "Commands.h"

dae::GameObjectCommand::GameObjectCommand(GameObject& object) noexcept
	:m_Object{object}
{
}

dae::MoveCommand::MoveCommand(GameObject& object, int x, int y) noexcept
	: GameObjectCommand(object)
	// Get Movement controller reference , MovementComponent{ object.GetComponent<MovementComponent>() }
	, m_Xdirection{ x }
	, m_Ydirection{ y }
{
	//TODO: Create MovementComponent
	//TODO: Finish implementing this
	//TODO: Put Commands in inpotManager
}

void dae::MoveCommand::Execute()
{
	//MovementController.Move(int x, int y);
}
