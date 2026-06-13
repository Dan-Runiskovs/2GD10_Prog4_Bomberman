#include "Blast.h"
#include "Timer.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "Scene.h"
#include <string>

dae::Blast::Blast(Scene& scene, GridCell& cell, int size, Orientation orientation, dae::Utils::PlayerColors color)
	: m_HostCell{ cell }
{
	auto go{ std::make_unique<GameObject>() };
	// --- Position : must be grid clamped ---
	go->GetComponent<dae::TransformComponent>().SetWorldPosition(cell.center);
	// --- Make it physical ---
	const glm::vec2 dimensions{ static_cast<float>(size), static_cast<float>(size) };
	m_PC = &go->AddComponent<dae::PhysicsComponent>(scene, dimensions, 0.f, true);
	// --- Add texture ---
	constexpr const char* Colors[]
	{
		"_r",
		"_g",
		"_b",
		"_y",
		"_m"
	};
	std::string colorText{ Colors[static_cast<int>(color)] };

	std::string orientationText{};
	switch (orientation)
	{
	case dae::Blast::Orientation::Central:
		orientationText = "_c.png";
		break;
	case dae::Blast::Orientation::Vertical:
		orientationText = "_v.png";
		break;
	case dae::Blast::Orientation::Horizontal:
		orientationText = "_h.png";
		break;
	default:
		break;
	}
	go->AddComponent<dae::RenderComponent>();
	go->GetComponent<dae::RenderComponent>().SetTexture("Blast/blast" + colorText + orientationText);
	go->GetComponent<dae::RenderComponent>().SetCentered(true);
	go->GetComponent<dae::RenderComponent>().SetDimensions(dimensions.x, dimensions.y);

	m_pGameObject = go.get();

	using OccupantType = dae::GridCell::OccupantType;
	using CellType = dae::CellType;
	m_HostCell.occupants.push_back({ m_pGameObject, OccupantType::Blast });

	scene.Add(std::move(go));

	for (auto& occ : cell.occupants)
	{
		if (occ.type == OccupantType::Barrel)
		{
			// --- 1. Delete barrel ---
			occ.pObject->MarkForDelete();

			cell.type = CellType::Free;

			std::erase_if(
				cell.occupants,
				[](const auto& occ)
				{
					return occ.type == OccupantType::Barrel;
				});
			break;
		}
	}
}

bool dae::Blast::DoesCollide(const SDL_FRect& other)
{
	if (m_HasExpired) return false;

	const auto myBounds{ m_PC->GetBounds() };
	return PhysicsComponent::DoesIntersect(myBounds, other);
}

void dae::Blast::Update()
{
	if (m_HasExpired)return;

	// --- Timer ---
	const auto elapsedSec{ Timer::GetInstance().GetElapsed() };
	m_Timer -= elapsedSec;

	if (m_Timer <= 0.f)
	{
		m_HasExpired = true;
		m_pGameObject->MarkForDelete();
		std::erase_if(
			m_HostCell.occupants,
			[this](const auto& occ)
			{
				return m_pGameObject == occ.pObject;
			});
	}
}
