#pragma once

#include "Commands.h"
#include "State.h"
#include "Game.h"

#include <memory>
#include <functional>
#include <utility>

namespace dae
{
	class HealthComponent;
	class DamageCommand final : public GameObjectCommand
	{
	public:
		DamageCommand(GameObject& object, uint8_t damage) noexcept;

		void Execute() override;
	private:
		HealthComponent& m_Health;
		uint8_t m_Damage;
	};

	class ScoreManager;
	class IncreaseScoreCommand final : public Command
	{
	public:
		IncreaseScoreCommand(ScoreManager& sm, uint8_t score) noexcept;

		void Execute() override;
	private:
		ScoreManager& m_ScoreManager;

		uint8_t m_Score;
	};

	class ChangeStateCommand final : public Command
	{
	public:

		ChangeStateCommand(Game& game, std::unique_ptr<GameState> state);

		void Execute() override;

	private:
		Game& m_Game;
		std::unique_ptr<GameState> m_pState;
	};

	class ExecuteCallbackCommand final : public Command
	{
	public:
		using Callback = std::function<void()>;
		template<typename Func, typename... Args>
		explicit ExecuteCallbackCommand(Func&& func, Args&&... args)
		{
			m_Callback =
				std::bind(
					std::forward<Func>(func),
					std::forward<Args>(args)...
				);
		}

		void Execute() override;
	private:
		Callback m_Callback;
	};
}