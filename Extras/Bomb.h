#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include "Utils.h"
#include "LevelGrid.h"

namespace dae
{
	class Scene;
	class Player;
	class GameObject;
	class PhysicsComponent;
	class Bomb final
	{
	public:
		Bomb(Scene& scene, GridCell& cell, int size, uint8_t blastRange, Player& owner);

		void Update();
		void Explode();
		bool HasExploded() const { return m_HasExploded; }
		dae::Utils::PlayerColors GetOwnerColor() const;
		const glm::vec2& GetPosition() const { return m_HostCell.center; }
		uint8_t GetBlastRange() { return m_BlastRange; }

	private:
		GridCell& m_HostCell;
		float m_Timer{ 3.f };

		uint8_t m_BlastRange{ 1 };
		bool m_HasExploded{ false };
		bool m_OwnerHasExited{ false };
		Player& m_Owner;

		GameObject* m_GameObject{};
		PhysicsComponent* m_PC{};
		
	};
}