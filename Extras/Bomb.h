#pragma once
#include <cstdint>
#include <glm/glm.hpp>

namespace dae
{
	class Scene;
	class Player;
	class GameObject;
	class PhysicsComponent;
	class Bomb final
	{
	public:
		Bomb(Scene& scene, const glm::vec2& position, int size, uint8_t blastRange, Player& owner);

		void Update();
		void Explode();
		bool HasExploded() const { return m_HasExploded; }
	private:
		float m_Timer{ 3.f };

		uint8_t m_BlastRange{ 1 };
		bool m_HasExploded{ false };
		bool m_OwnerHasExited{ false };
		Player& m_Owner;
		Scene& m_Scene;

		GameObject* m_GameObject{};
		PhysicsComponent* m_PC{};
	};
}