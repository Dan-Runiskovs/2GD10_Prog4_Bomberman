#include "Bomberman.h"
#include <cstdint>

// --- components ---
#include "FPSComponent.h"
#include "RenderComponent.h"
#include "TextComponent.h"
#include "TransformComponent.h"
#include "PhysicsComponent.h"
#include "HealthComponent.h"
#include "Commands.h"

// --- essential ---
#include "ResourceManager.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "SoundSystem.h"
#include "ServiceLocator.h"
#include "Renderer.h"
#include "CustomCommands.h"

//TODO: Remove in production
#include <iostream>

void dae::Bomberman::Init()
{
	// --- Load test SFX ---
	SoundData click{ 0, "Click.mp3" };
	ServiceLocator::GetSoundSystem().LoadSFX(click);

	SoundData skip{ 1, "Skip.mp3" };
	ServiceLocator::GetSoundSystem().LoadSFX(skip);


	m_GameStateStack.PushState(std::make_unique<TitleState>(*this));
	m_GameStateStack.ProcessPendingChanges();
}

void dae::Bomberman::HandleInput()
{
	m_GameStateStack.HandleInput();
}

void dae::Bomberman::Update()
{
	m_GameStateStack.Update();

	m_GameStateStack.ProcessPendingChanges();
}

void dae::Bomberman::Render()
{

}


