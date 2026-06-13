#pragma once
#include "Player.h"
#include <cstdint>
#include <vector>
#include <memory>


namespace dae
{
	class Scene;
	class GameObject;
	class PhysicsComponent;
	struct GridCell;
	class Upgrade final
	{
	public:
		enum class UpgradeType : uint8_t
		{
			None,
			BombCount,
			BlastIncrease,
			Speed,
			Exit
		};
		explicit Upgrade(Scene& scene, GridCell& hostCell, UpgradeType type);

		void Update(std::vector<std::unique_ptr<dae::Player>>& players);
		bool IsCollected() const;
	private:
		GameObject* m_pGameObject;
		PhysicsComponent* m_PC;
		UpgradeType m_UpgradeType;
		GridCell& m_HostCell;
	};
}