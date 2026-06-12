#pragma once
#include <cstdint>
#include <glm/glm.hpp>

namespace dae
{
	class Scene;
	class PhysicsComponent;
	class GameObject;
	class Player final
	{
	public:
		explicit Player(Scene& scene, const glm::vec2& origin, const glm::vec2& dimensions, float spriteSize, int playerIndex);
		~Player();

		void AddBomb();
		void AddRange();
		void AddSpeed(float margin);

		void PlaceBomb();
		void OnBombExploded();

		uint8_t GetActiveBombCount() const;
		uint8_t GetBlastRange() const;

	private:
		uint8_t m_MaxBombs{ 1 };
		uint8_t m_CurrentBombs{ 0 };
		uint8_t m_BlastRange{ 1 };

		PhysicsComponent* m_PhysicsComponent{};
		GameObject* m_GameObject{};
		float m_MoveSpeed{ 100.f };
	};
}