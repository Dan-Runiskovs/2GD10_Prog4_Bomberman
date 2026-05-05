#include "CustomCommands.h"
#include "GameObject.h"
#include "HealthComponent.h"
#include "ScoreManager.h"
#include "Bomberman.h"
#include "SoundSystem.h"
#include "ServiceLocator.h"

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

dae::IncreaseScoreCommand::IncreaseScoreCommand(ScoreManager& sm, uint8_t score) noexcept
	: m_ScoreManager{ sm }
	, m_Score{ score }
{
}

void dae::IncreaseScoreCommand::Execute()
{
	m_ScoreManager.AddScore(static_cast<unsigned int>(m_Score));
}

dae::ChangeStateCommand::ChangeStateCommand(Game& game, std::unique_ptr<GameState> state)
	: m_Game(game)
	, m_pState(std::move(state))
{
}

void dae::ChangeStateCommand::Execute()
{
	auto& bomberman{ static_cast<dae::Bomberman&>(m_Game) };
	ServiceLocator::GetSoundSystem().PlaySFX(0, 10);
	bomberman.GetGameStateStack().ChangeState(
		std::move(m_pState)
	);
}

void dae::ExecuteCallbackCommand::Execute()
{
	if (m_Callback) m_Callback();
}
