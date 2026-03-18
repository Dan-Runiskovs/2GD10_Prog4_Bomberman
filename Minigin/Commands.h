#pragma once
#include <cstdint>

namespace dae
{
	class GameObject;
	// --- Base ---
	class Command
	{
	public:
		virtual ~Command() noexcept = default;
		virtual void Execute() = 0;
	};

	class GameObjectCommand : public Command
	{
	public:
		virtual ~GameObjectCommand() noexcept = default;
	protected:
		GameObjectCommand(GameObject& object) noexcept;
		GameObject& GetGameObject() const { return m_Object; }
	private:
		GameObject& m_Object;
	};

	// --- Derived ---
	class PhysicsComponent;
	class MoveCommand final : public GameObjectCommand
	{
	public:
		MoveCommand(GameObject& object, int8_t x, int8_t y) noexcept;

		void Execute() override;
	private:
		PhysicsComponent& m_Movement;
		int8_t m_DirX{};
		int8_t m_DirY{};
		
		//direction is a vector
	};

	// TEMPORARY!!
	// TODO: Remove in production, health can not be known in the engine!
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

	// TEMPORARY!!
	// TODO: Remove in production, health can not be known in the engine!
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
}