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

void dae::Bomberman::CreateTitleScreen()
{
    auto& scene{ SceneManager::GetInstance().CreateScene() };

    // --- Background ---
    auto go{ std::make_unique<dae::GameObject>() };
    go->AddComponent<dae::RenderComponent>();
    go->GetComponent<dae::RenderComponent>().SetTexture("background.png");
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(0.f, 0.f);
    scene.Add(std::move(go));

    // --- Logo ---
    go = std::make_unique<dae::GameObject>();
    go->AddComponent<dae::RenderComponent>();
    go->GetComponent<dae::RenderComponent>().SetTexture("logo.png");
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(358.f, 180.f);
    scene.Add(std::move(go));

    // --- Title ---
    auto font{ dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36) };
    go = std::make_unique<dae::GameObject>();
    go->AddComponent<dae::RenderComponent>();
    go->AddComponent<dae::TextComponent>("Title Screen", font);
    go->GetComponent<dae::TextComponent>().SetColor({ 255, 255, 255, 255 });
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(292.f, 20.f);
    scene.Add(std::move(go));

#ifdef _DEBUG
    // --- FPS ---
    go = std::make_unique<dae::GameObject>();
    go->AddComponent<dae::RenderComponent>();
    go->AddComponent<dae::TextComponent>("TEMP", font);
    go->GetComponent<dae::TextComponent>().SetColor({ 255, 0, 0, 255 });
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(20.f, 20.f);
    go->AddComponent<dae::FPSComponent>();
    scene.Add(std::move(go));
#endif // _DEBUG

    // --- Helper Text ---
    font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 20);
    go = std::make_unique<dae::GameObject>();
    go->AddComponent<dae::RenderComponent>();
    go->AddComponent<dae::TextComponent>("Press A to enter game", font);
    go->GetComponent<dae::TextComponent>().SetColor({ 255, 0, 0, 255 });
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(20.f, 450.f);
    scene.Add(std::move(go));

    // --- Bindings ---
    auto& controllerRef = dae::InputManager::GetInstance().AddController(static_cast<uint8_t>(0));

    dae::InputManager::GetInstance().AddBinding(
        std::make_unique<ControllerBinding>(
            controllerRef, ControllerButton::GAMEPAD_A,

            std::make_unique<dae::ChangeStateCommand>(
                *this,
                std::make_unique<dae::MainMenuState>(*this)
            ),

            CommandType::OnRelease
        )
    );
}

void dae::Bomberman::CreateMainMenu()
{
    auto& scene{ SceneManager::GetInstance().CreateScene() };

    // --- Background ---
    auto go{ std::make_unique<dae::GameObject>() };
    go->AddComponent<dae::RenderComponent>();
    go->GetComponent<dae::RenderComponent>().SetTexture("background.png");
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(0.f, 0.f);
    scene.Add(std::move(go));

    // --- Logo ---
    go = std::make_unique<dae::GameObject>();
    go->AddComponent<dae::RenderComponent>();
    go->GetComponent<dae::RenderComponent>().SetTexture("logo.png");
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(358.f, 180.f);
    scene.Add(std::move(go));

    // --- Title ---
    auto font{ dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36) };
    go = std::make_unique<dae::GameObject>();
    go->AddComponent<dae::RenderComponent>();
    go->AddComponent<dae::TextComponent>("Main Menu", font);
    go->GetComponent<dae::TextComponent>().SetColor({ 255, 255, 255, 255 });
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(292.f, 20.f);
    scene.Add(std::move(go));

#ifdef _DEBUG
    // --- FPS ---
    go = std::make_unique<dae::GameObject>();
    go->AddComponent<dae::RenderComponent>();
    go->AddComponent<dae::TextComponent>("TEMP", font);
    go->GetComponent<dae::TextComponent>().SetColor({ 255, 0, 0, 255 });
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(20.f, 20.f);
    go->AddComponent<dae::FPSComponent>();
    scene.Add(std::move(go));
#endif // _DEBUG

    // --- Buttons ---
    // Clear any previous:
    m_SceneButtons.clear();
    // Add new :
    m_SceneButtons.push_back(Button{ {60.f, 400.f}, "Play", scene });
    m_SceneButtons.push_back(Button{ {60.f, 450.f}, "Leaderboard", scene });
    m_SceneButtons.push_back(Button{ {60.f, 500.f}, "Quit", scene });
    // Assign Callbacks:
    m_SceneButtons[0].GetSubject().AddObserver(
        [&](Event event)
        {
            switch (event)
            {
            case dae::Event::OnClick:
                std::cout << "Bomberman: Opening gamemode selection!\n";
                // TODO: Launch Select Gamemode
                break;
            default:
                break;
            }

        }
    );
    m_SceneButtons[1].GetSubject().AddObserver(
        [&](Event event)
        {
            switch (event)
            {
            case dae::Event::OnClick:
                std::cout << "Bomberman: Opening Leaderboard!\n";
                // TODO: Open Leaderboard
                break;
            default:
                break;
            }

        }
    );
    m_SceneButtons[2].GetSubject().AddObserver(
        [&](Event event)
        {
            switch (event)
            {
            case dae::Event::OnClick:
                std::cout << "Bomberman: Exit Queued!\n";
                InputManager::GetInstance().QueueExit();
                break;
            default:
                break;
            }

        }
    );
    // Select Default:
    m_SelectedButtonIndex = 0;
    m_SceneButtons[m_SelectedButtonIndex].SetSelected(true);

    // --- Bindings ---
    auto& controllerRef = dae::InputManager::GetInstance().AddController(static_cast<uint8_t>(0));
    auto& input = InputManager::GetInstance();
    input.AddBinding(std::make_unique<dae::ControllerBinding>(
        controllerRef, dae::ControllerButton::GAMEPAD_DPAD_DOWN,
        std::make_unique<dae::ExecuteCallbackCommand>([this]() {
            RotateButtonSelection(true);
            }),
        dae::CommandType::OnPress));
    input.AddBinding(std::make_unique<dae::ControllerBinding>(
        controllerRef, dae::ControllerButton::GAMEPAD_DPAD_UP,
        std::make_unique<dae::ExecuteCallbackCommand>([this]() {
            RotateButtonSelection(false);
            }),
        dae::CommandType::OnPress));
    input.AddBinding(std::make_unique<dae::ControllerBinding>(
        controllerRef, dae::ControllerButton::GAMEPAD_A,
        std::make_unique<dae::ExecuteCallbackCommand>([this](){
                m_SceneButtons[m_SelectedButtonIndex].Click();
            }),
        dae::CommandType::OnRelease));
}

void dae::Bomberman::RotateButtonSelection(bool isNext)
{
    // --- Early return if there is only one button ---
    if (m_SceneButtons.size() <= 1) return;

    // --- Deselect current one ---
    m_SceneButtons[m_SelectedButtonIndex].SetSelected(false);

    // --- Get Potential outcome ---
    int potentialIdx = (isNext)
        ? static_cast<int>(m_SelectedButtonIndex) + 1
        : static_cast<int>(m_SelectedButtonIndex) - 1;

    // --- Bounds check ---
    if (isNext)
    {
        if (potentialIdx >= static_cast<int>(m_SceneButtons.size())) potentialIdx = 0;
    }
    else
    {
        if (potentialIdx < 0) potentialIdx = static_cast<int>(m_SceneButtons.size() - 1);
    }

    // --- Assign and Select
    m_SelectedButtonIndex = static_cast<uint8_t>(potentialIdx);
    m_SceneButtons[m_SelectedButtonIndex].SetSelected(true);
}

