#include "LeaderboardState.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Utils.h"
#include "GameObject.h"
#include "Leaderboard.h"
#include "Bindings.h"
#include "CustomCommands.h"
#include "MainMenuState.h"

// --- Components ---
#include "RenderComponent.h"
#include "TextComponent.h"
#include "TransformComponent.h"

// --- Other ---
#include <iostream>

using dae::Utils::HexToSDLColor;
namespace HCol = dae::Utils::Colors;
using dae::Utils::Sounds;

dae::LeaderboardState::LeaderboardState(Game& game)
    :GameState(game)
{}

void dae::LeaderboardState::OnEnter()
{
    std::cout << "GameState: Leaderboard entered\n";
    CreateLeaderboardState();
}

void dae::LeaderboardState::OnExit()
{
    InputManager::GetInstance().ClearBindings();
    SceneManager::GetInstance().DestroyAllScenes();
    std::cout << "GameState: TitleScreenExited\n";
}

void dae::LeaderboardState::CreateLeaderboardState()
{
    // --- Data ---
    auto& scene{ SceneManager::GetInstance().CreateScene() };
    const auto windowSize{ Renderer::GetInstance().GetWindowSize() };
    const auto windowCentre{ glm::vec2(windowSize.x / 2.f, windowSize.y / 2.f) };

    // --- Fonts ---
    auto mainFont{ dae::ResourceManager::GetInstance().LoadFont("MainFont.ttf", 150) };
    auto subFont{ dae::ResourceManager::GetInstance().LoadFont("SubFont.ttf", 36) };

    // --- Background ---
    Renderer::GetInstance().SetBackgroundColor(HexToSDLColor(HCol::BLACK));

    // --- Title Background ---
    auto go{ std::make_unique<dae::GameObject>() };
    go->AddComponent<dae::RenderComponent>();
    go->AddComponent<dae::TextComponent>("Leaderboard", mainFont);
    go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::PALE_BROWN));
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(windowCentre.x + 5.f, 105.f);
    go->GetComponent<dae::RenderComponent>().SetCentered(true);
    scene.Add(std::move(go));

    // --- Title Foreground ---
    go = std::make_unique<dae::GameObject>();
    go->AddComponent<dae::RenderComponent>();
    go->AddComponent<dae::TextComponent>("Leaderboard", mainFont);
    go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::YELLOW));
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(windowCentre.x, 100.f);
    go->GetComponent<dae::RenderComponent>().SetCentered(true);
    scene.Add(std::move(go));

    // --- Leaderboard Entries ---
    constexpr float startY{ 250.f };
    constexpr float rowSpacing{ 50.f };

    const auto& leaderboard{ Leaderboard::GetInstance() };

    for (size_t entryIdx{}; entryIdx < leaderboard.GetEntryCount(); ++entryIdx)
    {
        const auto& entry{ leaderboard.GetEntries()[entryIdx] };

        std::string text
        {
            std::to_string(entryIdx + 1)
            + ". "
            + entry.initials[0] + entry.initials[1] + entry.initials[2]
            + "    "
            + std::to_string(entry.scoreHundreds) + "00"
        };

        auto row{ std::make_unique<dae::GameObject>() };

        row->AddComponent<dae::RenderComponent>();
        row->AddComponent<dae::TextComponent>(text, subFont);
        row->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::WHITE));
        row->GetComponent<dae::TransformComponent>().SetWorldPosition(
                windowCentre.x,
                startY + rowSpacing * static_cast<float>(entryIdx));
        row->GetComponent<dae::RenderComponent>().SetCentered(true);

        scene.Add(std::move(row));

        // --- Bindings ---
        auto& controllerRef = dae::InputManager::GetInstance().AddController(static_cast<uint8_t>(0));

        dae::InputManager::GetInstance().AddBinding(
            std::make_unique<ControllerBinding>(
                controllerRef, ControllerButton::GAMEPAD_B,
                std::make_unique<dae::ChangeStateCommand>(m_Game,
                    std::make_unique<dae::MainMenuState>(m_Game)
                ),
                CommandType::OnRelease)
        );
    }
}