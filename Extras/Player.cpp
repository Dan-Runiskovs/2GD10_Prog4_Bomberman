#include "Player.h"
#include "GameObject.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "PhysicsComponent.h"
#include "InputManager.h"
#include "Utils.h"
#include "Scene.h"
#include <cassert>
#include <memory>
#include <string>
#include <iostream>

dae::Player::Player(Scene& scene, const glm::vec2& origin, const glm::vec2& dimensions, float spriteSize, int playerIndex)
{
	assert(playerIndex < 4u);
	auto go{ std::make_unique<GameObject>() };
    // --- Correct position ---
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(origin);
    // --- Physics ---
    m_PhysicsComponent = &go->AddComponent<dae::PhysicsComponent>(scene, dimensions, m_MoveSpeed, false);
    // --- Set texture ---
    go->AddComponent<dae::RenderComponent>();
    go->GetComponent<dae::RenderComponent>().SetTexture("Character/player_" + std::to_string(playerIndex) + ".png");
    go->GetComponent<dae::RenderComponent>().SetCentered(true);
    go->GetComponent<dae::RenderComponent>().SetDimensions(spriteSize, spriteSize);
    // --- Input ---
	auto& bomberRef = *go;
	auto& controllerRef = dae::InputManager::GetInstance().AddController(static_cast<uint8_t>(playerIndex));
	auto& input = dae::InputManager::GetInstance();
	input.AddBinding(std::make_unique<dae::ControllerBinding>(
		controllerRef,
		dae::ControllerButton::GAMEPAD_DPAD_RIGHT,
		std::make_unique<dae::MoveCommand>(bomberRef, 1, 0, spriteSize * 2.f),
		dae::CommandType::OnHold));
	input.AddBinding(std::make_unique<dae::ControllerBinding>(
		controllerRef,
		dae::ControllerButton::GAMEPAD_DPAD_LEFT,
		std::make_unique<dae::MoveCommand>(bomberRef, -1, 0, spriteSize * 2.f),
		dae::CommandType::OnHold));
	input.AddBinding(std::make_unique<dae::ControllerBinding>(
		controllerRef,
		dae::ControllerButton::GAMEPAD_DPAD_UP,
		std::make_unique<dae::MoveCommand>(bomberRef, 0, -1, spriteSize * 2.f),
		dae::CommandType::OnHold));
	input.AddBinding(std::make_unique<dae::ControllerBinding>(
		controllerRef,
		dae::ControllerButton::GAMEPAD_DPAD_DOWN,
		std::make_unique<dae::MoveCommand>(bomberRef, 0, 1, spriteSize * 2.f),
		dae::CommandType::OnHold));

	// TODO: Only movement for now, later DropBombs on "A"
	m_GameObject = go.get();
	scene.Add(std::move(go));
}

dae::Player::~Player()
{
}

void dae::Player::AddBomb()
{
    ++m_MaxBombs;
}

void dae::Player::AddRange()
{
    ++m_BlastRange;
}

void dae::Player::AddSpeed(float margin)
{
    m_MoveSpeed += margin;
	m_PhysicsComponent->SetSpeed(m_MoveSpeed);
}

bool dae::Player::TryPlaceBomb()
{
	if (m_CurrentBombs >= m_MaxBombs) return false;

	++m_CurrentBombs;
	std::cout << "Placing bomb!\n";
	return true;
}

void dae::Player::OnBombExploded()
{
	if (m_CurrentBombs > 0) --m_CurrentBombs;
}

const glm::vec2& dae::Player::GetWorldPos() const
{
	return m_PhysicsComponent->GetOrigin();
}

SDL_FRect dae::Player::GetBounds() const
{
	return m_PhysicsComponent->GetBounds();
}

uint8_t dae::Player::GetActiveBombCount() const
{
    return m_CurrentBombs;
}

uint8_t dae::Player::GetBlastRange() const
{
    return m_BlastRange;
}
