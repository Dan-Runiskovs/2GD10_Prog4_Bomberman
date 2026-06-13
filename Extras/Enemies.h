#pragma once
#include "Interface.h"
#include <cstdint>
#include <memory>
#include <glm/glm.hpp>
#include "Subject.h"
#include "Player.h"

namespace dae
{
	class PhysicsComponent;
	class GameObject;
	class Scene;
	class LevelGrid;

	// --- Virtual Baseclass
	class Enemy : public IICollideable
	{
	public:
		virtual ~Enemy() = default;

		virtual void Update(std::vector<std::unique_ptr<dae::Player>>&) = 0;

		bool IsAlive() const { return m_IsAlive; }
		bool DoesCollide(const SDL_FRect& other) override;

		virtual void Kill();
		Subject& GetSubject() { return m_Subject; }
		uint8_t GetScoreValue() const { return m_ScoreValue; }
	protected:
		PhysicsComponent* m_PC{};
		GameObject* m_pGameObject{};
		uint8_t m_ScoreValue{ 0 };
		Subject m_Subject{};

		bool m_IsAlive{ true };
		float m_MoveSpeed{ 50.f };
	};

	class Balloom final : public Enemy
	{
	public:
		enum class Direction : uint8_t
		{
			Up,
			Down,
			Left,
			Right
		};

		Balloom(Scene& scene, LevelGrid& level,	const glm::vec2& spawnPos, float size);

		void Update(std::vector<std::unique_ptr<dae::Player>>& players) override;

	private:
		void PickNewDirection();

		LevelGrid& m_Level;

		Direction m_CurrentDirection
		{
			Direction::Down
		};

		float m_DirectionCooldown{};
	};
}