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
	class MovementComponent;
	class MoveCommand final : public GameObjectCommand
	{
	public:
		MoveCommand(GameObject& object, int8_t x, int8_t y) noexcept;

		void Execute() override;
	private:
		MovementComponent& m_Movement;
		int8_t m_DirX{};
		int8_t m_DirY{};
	};
}