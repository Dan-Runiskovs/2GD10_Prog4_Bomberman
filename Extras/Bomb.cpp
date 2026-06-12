#include "Bomb.h"
#include "Player.h"
#include "GameObject.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "PhysicsComponent.h"
#include "Scene.h"
#include "Timer.h"
#include <memory>

dae::Bomb::Bomb(Scene& scene, const glm::vec2& position, int size, uint8_t blastRange, Player& owner)
	: m_BlastRange{ blastRange }
	, m_Owner{ owner }
	, m_Scene{ scene }
{
	auto go{ std::make_unique<GameObject>() };
	// --- Position : must be grid clamped ---
	go->GetComponent<dae::TransformComponent>().SetWorldPosition(position);
	// --- Make it physical ---
	const glm::vec2 dimensions{ static_cast<float>(size), static_cast<float>(size) };
	m_PC = &go->AddComponent<dae::PhysicsComponent>(scene, dimensions, 0.f, true);
	// --- Add texture ---
	go->AddComponent<dae::RenderComponent>();
	go->GetComponent<dae::RenderComponent>().SetTexture("Item/bomb.png");
	go->GetComponent<dae::RenderComponent>().SetCentered(true);
	go->GetComponent<dae::RenderComponent>().SetDimensions(dimensions.x, dimensions.y);

	m_GameObject = go.get();
	scene.Add(std::move(go));
}

void dae::Bomb::Update()
{
	// --- Timer ---
	const auto elapsedSec{ Timer::GetInstance().GetElapsed() };
	m_Timer -= elapsedSec;
	
	if (m_Timer <= 0.f and not m_HasExploded)
	{
		Explode();
		return;
	}

	// --- Handle Owner collision --- 
	if (!m_OwnerHasExited and not m_HasExploded)
	{
		const auto bombBounds{ m_PC->GetBounds() };
		const auto ownerBounds{ m_Owner.GetBounds() };
		if (!PhysicsComponent::DoesIntersect(bombBounds, ownerBounds))
		{
			m_OwnerHasExited = true;
			m_PC->SetTrigger(false);
		}
	}
}

void dae::Bomb::Explode()
{
	m_HasExploded = true;
	m_Owner.OnBombExploded();

	assert(m_GameObject);
	m_GameObject->MarkForDelete();

	// --- Create Blast ---
}
