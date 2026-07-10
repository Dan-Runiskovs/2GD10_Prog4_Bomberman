#include "MainMenuState.h"
#include "ResourceManager.h"
#include "Leaderboard.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Renderer.h"
#include "Utils.h"
#include "GameObject.h"
#include "GamemodeSelectionState.h"
#include "LeaderboardState.h"

// --- Components ---
#include "RenderComponent.h"
#include "TextComponent.h"
#include "TransformComponent.h"
#include "FPSComponent.h"

// --- Others ---
#include <iostream>

using dae::Utils::HexToSDLColor;
namespace HCol = dae::Utils::Colors;
using dae::Utils::Sounds;

dae::MainMenuState::MainMenuState(Game& game)
    :GameState(game)
{}

void dae::MainMenuState::OnEnter()
{
    std::cout << "GameState: Main menu entered\n";
    CreateMainMenu();
    auto path{ ResourceManager::GetInstance().GetDataPath() };
    path.append("Leaderboard.bin");
    Leaderboard::GetInstance().TryLoadEntries(path);
}

void dae::MainMenuState::OnExit()
{
    InputManager::GetInstance().ClearBindings();
    SceneManager::GetInstance().DestroyAllScenes();
    m_SceneButtons.clear();
    std::cout << "GameState: Main menu exited\n";
}

void dae::MainMenuState::CreateMainMenu()
{
    // --- Data ---
    auto& scene{ SceneManager::GetInstance().CreateScene() };
    const auto windowSize{ Renderer::GetInstance().GetWindowSize() };
    const auto windowCentre{ glm::vec2(windowSize.x / 2.f, windowSize.y / 2.f) };

    // --- Fonts ---
    auto mainFont{ dae::ResourceManager::GetInstance().LoadFont("MainFont.ttf", 200) };
    auto subFont{ dae::ResourceManager::GetInstance().LoadFont("SubFont.ttf", 36) };

    // --- Background --- 
    Renderer::GetInstance().SetBackgroundColor(HexToSDLColor(HCol::BLACK));

    // --- Title Background ---
    auto go{ std::make_unique<dae::GameObject>() };
    go->AddComponent<dae::RenderComponent>();
    go->AddComponent<dae::TextComponent>("Bomberman", mainFont);
    go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::PALE_BROWN));
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(windowCentre.x + 5.f, windowCentre.y * .5f + 5.f);
    go->GetComponent<dae::RenderComponent>().SetCentered(true);
    scene.Add(std::move(go));

    // --- Title Foreground ---
    go = std::make_unique<dae::GameObject>();
    go->AddComponent<dae::RenderComponent>();
    go->AddComponent<dae::TextComponent>("Bomberman", mainFont);
    go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::YELLOW));
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(windowCentre.x, windowCentre.y * .5f);
    go->GetComponent<dae::RenderComponent>().SetCentered(true);
    scene.Add(std::move(go));

#ifdef _DEBUG
    // --- FPS ---
    auto debugFont{ dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 30) };
    go = std::make_unique<dae::GameObject>();
    go->AddComponent<dae::RenderComponent>();
    go->AddComponent<dae::TextComponent>("TEMP", debugFont);
    go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::DEBUG_RED));
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(20.f, 20.f);
    go->AddComponent<dae::FPSComponent>();
    scene.Add(std::move(go));
#endif // _DEBUG

    // --- Buttons ---
    // Clear any previous:
    m_SceneButtons.clear();
    // Add new :
    m_SceneButtons.push_back(Button{ {windowCentre.x, 380.f}, "Play", scene });
    m_SceneButtons.push_back(Button{ {windowCentre.x, 450.f}, "Leaderboard", scene });
    m_SceneButtons.push_back(Button{ {windowCentre.x, 520.f}, "Quit", scene });
    // Assign Callbacks:
    m_SceneButtons[0].GetSubject().AddObserver(
        [this](Event event)
        {
            switch (event)
            {
            case dae::Event::OnClick:
                std::cout << "Bomberman: Opening gamemode selection!\n";
                ChangeState(std::make_unique<dae::GamemodeSelectionMenuState>(m_Game));
                break;
            default:
                break;
            }

        }
    );
    m_SceneButtons[1].GetSubject().AddObserver(
        [this](Event event)
        {
            switch (event)
            {
            case dae::Event::OnClick:
                std::cout << "Bomberman: Opening Leaderboard!\n";
                std::cout << "Bomberman: WARNING: Not implemented yet!\n";
                ChangeState(std::make_unique<dae::LeaderboardState>(m_Game));
                break;
            default:
                break;
            }

        }
    );
    m_SceneButtons[2].GetSubject().AddObserver(
        [](Event event)
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
    CreateMenuBindings(true);
}