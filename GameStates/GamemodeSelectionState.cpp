#include "GamemodeSelectionState.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Utils.h"
#include "Bomberman.h"
#include "PreGameState.h"

// --- Components ---
#include "RenderComponent.h"
#include "TextComponent.h"
#include "TransformComponent.h"
#include "FPSComponent.h"

// --- Other ---
#include <iostream>

using dae::Utils::HexToSDLColor;
namespace HCol = dae::Utils::Colors;
using dae::Utils::Sounds;

dae::GamemodeSelectionMenuState::GamemodeSelectionMenuState(Game& game)
    :GameState(game)
{}

void dae::GamemodeSelectionMenuState::OnEnter()
{
    std::cout << "GameState: Gamemode selection menu entered\n";
    CreateGamemodeSelection();
}

void dae::GamemodeSelectionMenuState::OnExit()
{
    InputManager::GetInstance().ClearBindings();
    SceneManager::GetInstance().DestroyAllScenes();
    m_SceneButtons.clear();
    std::cout << "GameState: Gamemode selection menu exited\n";
}

void dae::GamemodeSelectionMenuState::CreateGamemodeSelection()
{
    auto& scene{ SceneManager::GetInstance().CreateScene() };
    const auto windowSize{ Renderer::GetInstance().GetWindowSize() };
    const auto windowCentre{ glm::vec2(windowSize.x / 2.f, windowSize.y / 2.f) };

    // --- Fonts ---
    auto mainFont{ dae::ResourceManager::GetInstance().LoadFont("MainFont.ttf", 150) };
    auto subFont{ dae::ResourceManager::GetInstance().LoadFont("SubFont.ttf", 36) };

    // --- Title Background ---
    auto go{ std::make_unique<dae::GameObject>() };
    go->AddComponent<dae::RenderComponent>();
    go->AddComponent<dae::TextComponent>("Select Gamemode", mainFont);
    go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::PALE_BROWN));
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(windowCentre.x + 5.f, windowCentre.y * .5f + 5.f);
    go->GetComponent<dae::RenderComponent>().SetCentered(true);
    scene.Add(std::move(go));

    // --- Title Foreground ---
    go = std::make_unique<dae::GameObject>();
    go->AddComponent<dae::RenderComponent>();
    go->AddComponent<dae::TextComponent>("Select Gamemode", mainFont);
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

    // --- Subtitle Text ---
    go = std::make_unique<dae::GameObject>();
    go->AddComponent<dae::RenderComponent>();
    go->AddComponent<dae::TextComponent>("Only SOLO counts for leaderboard", subFont);
    go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::GREY));
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(windowCentre.x, windowCentre.y * 1.9f);
    go->GetComponent<dae::RenderComponent>().SetCentered(true);
    scene.Add(std::move(go));

    // --- Buttons ---

    // Clear any previous:
    m_SceneButtons.clear();
    // Add new :
    m_SceneButtons.push_back(Button{ {windowCentre.x, 380.f}, "Solo", scene });
    m_SceneButtons.push_back(Button{ {windowCentre.x, 450.f}, "PvP", scene });
    m_SceneButtons.push_back(Button{ {windowCentre.x, 520.f}, "Co-op", scene });
    // Assign Callbacks:

    m_SceneButtons[0].GetSubject().AddObserver(
        [this](Event event)
        {
            switch (event)
            {
            case dae::Event::OnClick:
                std::cout << "Bomberman: Lauching Solo Game!\n";
                static_cast<dae::Bomberman&>(m_Game).GetMatchSession().SetMode(dae::MatchSession::GameMode::Solo);
                ChangeState(std::make_unique<dae::PreGameState>(m_Game, true));
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
                std::cout << "Bomberman: Launching PvP!\n";
                static_cast<dae::Bomberman&>(m_Game).GetMatchSession().SetMode(dae::MatchSession::GameMode::Pvp);
                ChangeState(std::make_unique<dae::PreGameState>(m_Game, false));
                break;
            default:
                break;
            }

        }
    );
    m_SceneButtons[2].GetSubject().AddObserver(
        [this](Event event)
        {
            switch (event)
            {
            case dae::Event::OnClick:
                std::cout << "Bomberman: Launching Co-op!\n";
                static_cast<dae::Bomberman&>(m_Game).GetMatchSession().SetMode(dae::MatchSession::GameMode::Coop);
                ChangeState(std::make_unique<dae::PreGameState>(m_Game, false));
                break;
            default:
                break;
            }

        }
    );
    CreateMenuBindings(true);
}