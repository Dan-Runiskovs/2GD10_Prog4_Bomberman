#include "State.h"
#include "Game.h"

#include "Bomberman.h"

#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"

#include "ResourceManager.h"
#include "InputManager.h"

#include "RenderComponent.h"
#include "TextComponent.h"
#include "TransformComponent.h"
#include "FPSComponent.h"

#include <iostream>

dae::GameState::GameState(Game& game)
	:m_Game(game)
{
}

#pragma region Title
dae::TitleState::TitleState(Game& game)
	: GameState(game)
{
}

void dae::TitleState::OnEnter()
{
    std::cout << "GameState: Entered TitleState\n";
    static_cast<dae::Bomberman&>(m_Game).CreateTitleScreen();
}

void dae::TitleState::OnExit()
{
    InputManager::GetInstance().ClearBindings();
    SceneManager::GetInstance().DestroyAllScenes();
    std::cout << "GameState: TitleScreenExited\n";
}

#pragma endregion

#pragma region MainMenu
dae::MainMenuState::MainMenuState(Game& game)
    :GameState(game)
{
}

void dae::MainMenuState::OnEnter()
{
    std::cout << "GameState: Main menu entered\n";
    static_cast<dae::Bomberman&>(m_Game).CreateMainMenu();
}

void dae::MainMenuState::OnExit()
{
    InputManager::GetInstance().ClearBindings();
    SceneManager::GetInstance().DestroyAllScenes();
    std::cout << "GameState: Main menu exited\n";
}

#pragma endregion