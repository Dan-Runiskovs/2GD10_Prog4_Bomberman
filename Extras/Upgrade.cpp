#include "Upgrade.h"
#include "GameObject.h"
#include "LevelGrid.h"
#include "Scene.h"
#include "TransformComponent.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include <string>
#include <glm/glm.hpp>

dae::Upgrade::Upgrade(Scene& scene, GridCell& hostCell, UpgradeType type)
	: m_UpgradeType{ type }
	, m_HostCell{ hostCell }
{
	auto go{ std::make_unique<GameObject>() };
	// --- Position : must be grid clamped ---
	go->GetComponent<dae::TransformComponent>().SetWorldPosition(m_HostCell.center);
	// --- Make it physical ---
	const auto size{ hostCell.m_CellSizePx };
	const glm::vec2 dimensions{ static_cast<float>(size), static_cast<float>(size) };
	m_PC = &go->AddComponent<dae::PhysicsComponent>(scene, glm::vec2{dimensions.x * 0.8f, dimensions.y * 0.8f}, 0.f, true);
	// --- Add texture ---
	std::string name{"Item/upgrade_"};
	switch (type)
	{
	case dae::Upgrade::UpgradeType::BombCount:
		name += "bomb.png";
		break;
	case dae::Upgrade::UpgradeType::BlastIncrease:
		name += "blast.png";
		break;
	case dae::Upgrade::UpgradeType::Speed:
		name += "speed.png";
		break;
	default:
		break;
	}
	go->AddComponent<dae::RenderComponent>();
	go->GetComponent<dae::RenderComponent>().SetTexture(name);
	go->GetComponent<dae::RenderComponent>().SetCentered(true);
	go->GetComponent<dae::RenderComponent>().SetDimensions(dimensions.x, dimensions.y);

	m_pGameObject = go.get();
	scene.Add(std::move(go));
}

void dae::Upgrade::Update(std::vector<std::unique_ptr<dae::Player>>& players)
{
	if (!m_pGameObject) return;

	// --- No updating unless no barrel ---
	if (m_HostCell.type == dae::CellType::Barrel) return;
	if (m_pGameObject->IsMarkedForDelete()) return;

	for(auto& player : players)
	{
		auto& playerRef{ *player.get() };
		const auto theseBounds{ m_PC->GetBounds() };
		const auto playerBounds{ playerRef.GetBounds() };
		if (PhysicsComponent::DoesIntersect(theseBounds, playerBounds))
		{
			// --- Collides! => "pick up" ---
			switch (m_UpgradeType)
			{
			case dae::Upgrade::UpgradeType::BombCount:
				playerRef.AddBomb();
				break;
			case dae::Upgrade::UpgradeType::BlastIncrease:
				playerRef.AddRange();
				break;
			case dae::Upgrade::UpgradeType::Speed:
				playerRef.AddSpeed();
				break;
			case dae::Upgrade::UpgradeType::None:
			default:
				break;
			}

			std::erase_if(
				m_HostCell.occupants,
				[this](const auto& occ)
				{
					return occ.pObject == m_pGameObject;
				});
			m_pGameObject->MarkForDelete();
			return;
		}
	}
}

bool dae::Upgrade::IsCollected() const
{
	return m_pGameObject == nullptr || m_pGameObject->IsMarkedForDelete();
}
