#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <SDL3/SDL.h>
#include "Subject.h"
#include "Interface.h"

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
		void AddSpeed(float margin = 50.f);

		bool TryPlaceBomb();
		void OnBombExploded();
		const glm::vec2& GetWorldPos() const;
		SDL_FRect GetBounds() const;
		Subject& GetOnStateChanged() { return m_OnStateChanged; }
		void Kill();

		bool IsAlive() const { return m_IsAlive; }
		uint8_t GetActiveBombCount() const;
		uint8_t GetBlastRange() const;
		uint8_t GetPlayerIndex() const;

	private:
		uint8_t m_MaxBombs{ 1 };
		uint8_t m_CurrentBombs{ 0 };
		uint8_t m_BlastRange{ 1 };
		uint8_t m_PlayerIndex{ 0 };

		PhysicsComponent* m_PhysicsComponent{};
		GameObject* m_GameObject{};
		Subject m_OnStateChanged{};
		Subject m_OnWin{};

		bool m_IsAlive{ true };
		float m_MoveSpeed{ 100.f };
	};
}