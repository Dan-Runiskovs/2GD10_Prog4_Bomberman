#pragma once

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
	class MoveCommand final : public GameObjectCommand
	{
	public:
		MoveCommand(GameObject& pObject, int x, int y) noexcept;

		void Execute() override;
	private:
		int m_Xdirection{};
		int m_Ydirection{};
	};
}
