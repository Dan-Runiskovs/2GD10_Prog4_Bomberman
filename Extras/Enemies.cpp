#include "Enemies.h"
#include "GameObject.h"
#include "Scene.h"
#include "LevelGrid.h"
#include "PhysicsComponent.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include <random>
#include <algorithm>
#include <iostream>

dae::Balloom::Balloom(Scene& scene, LevelGrid& level, const glm::vec2& spawnPos, float size)
	:m_Level{level}
{
	// --- Overrides ---
	m_ScoreValue = 1u;

	auto go{ std::make_unique<GameObject>() };
	// --- Correct position ---
	go->GetComponent<dae::TransformComponent>().SetWorldPosition(spawnPos);
	// --- Physics ---
	const glm::vec2 dimensions{ (size * 0.8f),(size * 0.8f) };
	m_PC = &go->AddComponent<dae::PhysicsComponent>(scene, dimensions, m_MoveSpeed, true);
	// --- Set texture ---
	go->AddComponent<dae::RenderComponent>();
	go->GetComponent<dae::RenderComponent>().SetTexture("Character/Balloom.png");
	go->GetComponent<dae::RenderComponent>().SetCentered(true);
	go->GetComponent<dae::RenderComponent>().SetDimensions(size, size);

	m_pGameObject = go.get();
	scene.Add(std::move(go));
}

void dae::Balloom::Update(std::vector<std::unique_ptr<dae::Player>>& players)
{
	if (!m_IsAlive) return;
	if (!m_pGameObject) return;
	if (m_pGameObject->IsMarkedForDelete()) return;

	// --- 0. Process world collisions
	const auto& thisCell{ m_Level.WorldPosToGridCell(m_PC->GetOrigin()) };
	for (auto& occ : thisCell.occupants)
	{
		if (occ.type == GridCell::OccupantType::Blast)
		{
			this->Kill();
			return;
		}
	}

	// --- 1. Process Collisions ---
	for (auto& player : players)
	{
		auto& playerRef{ *player.get() };
		if (!playerRef.IsAlive()) continue;

		const auto theseBounds{ m_PC->GetBounds() };
		const auto playerBounds{ playerRef.GetBounds() };

		if (PhysicsComponent::DoesIntersect(theseBounds, playerBounds))
		{
			playerRef.Kill();
			continue;
		}
	}
	// --- 2.Move ---
	glm::vec2 dir{};
	switch (m_CurrentDirection)
	{
	case Direction::Up:
		dir = { 0.f, -1.f };
		break;

	case Direction::Down:
		dir = { 0.f, 1.f };
		break;

	case Direction::Left:
		dir = { -1.f, 0.f };
		break;

	case Direction::Right:
		dir = { 1.f, 0.f };
		break;
	}

	if (!m_PC->TryMove(dir, 100.f))
	{
		PickNewDirection();
	}
}

void dae::Balloom::PickNewDirection()
{
	std::vector<Direction> validDirections;

	const auto& cell{ m_Level.WorldPosToGridCell(m_PC->GetOrigin()) };

	const int x{ cell.x };
	const int y{ cell.y };

	auto TryAddDirection =
		[&](int nx, int ny, Direction dir)
		{
			if (nx < 0 ||
				ny < 0 ||
				nx >= m_Level.GetWidth() ||
				ny >= m_Level.GetHeight())
			{
				return;
			}

			const auto& neighbour
			{
				m_Level.At(
					static_cast<uint8_t>(nx),
					static_cast<uint8_t>(ny))
			};

			if (neighbour.type != CellType::Wall &&
				neighbour.type != CellType::Barrel)
			{
				validDirections.push_back(dir);
			}
		};

	TryAddDirection(x + 1, y, Direction::Right);
	TryAddDirection(x - 1, y, Direction::Left);
	TryAddDirection(x, y + 1, Direction::Down);
	TryAddDirection(x, y - 1, Direction::Up);

	if (validDirections.empty()) return;

	static std::random_device rd;
	static std::default_random_engine eng(rd());

	std::uniform_int_distribution<size_t> dist
	{
		0,
		validDirections.size() - 1
	};

	m_CurrentDirection = validDirections[dist(eng)];
}

bool dae::Enemy::DoesCollide(const SDL_FRect& other)
{
	if (!m_IsAlive) return false;

	const auto myBounds{ m_PC->GetBounds() };
	return PhysicsComponent::DoesIntersect(myBounds, other);
}

void dae::Enemy::Kill()
{
	if (!m_IsAlive) return;

	m_IsAlive = false;

	m_Subject.Notify(Event::OnDeath);

	if (m_pGameObject)
	{
		m_pGameObject->MarkForDelete();
	}
}
