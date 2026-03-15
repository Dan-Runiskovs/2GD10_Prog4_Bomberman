#include "Commands.h"
#include "GameObject.h"
#include "PhysicsComponent.h"

//TODO: Remove in production
#include "HealthComponent.h"
#include "ScoreManager.h"

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

dae::DamageCommand::DamageCommand(GameObject& object, uint8_t damage) noexcept
	: GameObjectCommand(object)
	, m_Health{ object.GetComponent<HealthComponent>() }
	, m_Damage{ damage }
{
}

void dae::DamageCommand::Execute()
{
	m_Health.TryTakeDamage(m_Damage);
}

dae::IncreaseScoreCommand::IncreaseScoreCommand(uint8_t score) noexcept
	: m_Score{score}
{
}

void dae::IncreaseScoreCommand::Execute()
{
	ScoreManager::GetInstance().AddScore(static_cast<unsigned int>(m_Score));
}
