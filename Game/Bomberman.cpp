#include "Bomberman.h"

// --- essential ---
#include "SoundSystem.h"
#include "ServiceLocator.h"
#include "../GameStates/TitleState.h"

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


