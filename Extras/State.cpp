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

#include "Commands.h"
#include "CustomCommands.h"
#include "ServiceLocator.h"
#include "SoundSystem.h"
#include "Renderer.h"
#include "Utils.h"
#include "Timer.h"
#include "Subject.h"
#include "Camera.h"
#include "Leaderboard.h"

#include <iostream>
#include <cassert>
#include <cmath>
#include <filesystem>


// TODO: Temp:
#include <random>

// TODO: Idea:
/*
MAKE 2 different game scenes
PVP vs Regular
Where regular get's an argument of players present
-> (active controllers) and creates player's and accordingly;
-> Preset positions 1/2/3/4 are serialized in the level;
*/

using dae::Utils::HexToSDLColor;
namespace HCol = dae::Utils::Colors;
using dae::Utils::Sounds;

dae::GameState::GameState(Game& game)
	: m_Game(game)
{
}

#pragma region Helpers
void dae::GameState::RotateButtonSelection(bool isNext)
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

void dae::GameState::CreateMenuBindings(bool isVertical)
{
    // --- Select Default ---
    m_SelectedButtonIndex = 0;
    m_SceneButtons[m_SelectedButtonIndex].SetSelected(true);

    // --- Bindings ---
    auto& controllerRef = dae::InputManager::GetInstance().AddController(static_cast<uint8_t>(0));
    auto& input = InputManager::GetInstance();
    input.AddBinding(std::make_unique<dae::ControllerBinding>(
        controllerRef, (isVertical) ? dae::ControllerButton::GAMEPAD_DPAD_DOWN : dae::ControllerButton::GAMEPAD_DPAD_RIGHT,
        std::make_unique<dae::ExecuteCallbackCommand>([this]() {
            RotateButtonSelection(true);
            ServiceLocator::GetSoundSystem().PlaySFX(uint8_t(Sounds::Skip), 10);
            }),
        dae::CommandType::OnPress));
    input.AddBinding(std::make_unique<dae::ControllerBinding>(
        controllerRef, (isVertical) ? dae::ControllerButton::GAMEPAD_DPAD_UP : dae::ControllerButton::GAMEPAD_DPAD_LEFT,
        std::make_unique<dae::ExecuteCallbackCommand>([this]() {
            RotateButtonSelection(false);
            ServiceLocator::GetSoundSystem().PlaySFX(uint8_t(Sounds::Skip), 10);
            }),
        dae::CommandType::OnPress));
    input.AddBinding(std::make_unique<dae::ControllerBinding>(
        controllerRef, dae::ControllerButton::GAMEPAD_A,
        std::make_unique<dae::ExecuteCallbackCommand>([this]() {
            m_SceneButtons[m_SelectedButtonIndex].Click();
            ServiceLocator::GetSoundSystem().PlaySFX(uint8_t(Sounds::Select), 10);
            }),
        dae::CommandType::OnRelease));
}

void dae::GameState::ChangeState(std::unique_ptr<GameState> newState)
{
    m_Game.GetGameStateStack().ChangeState(std::move(newState));
}
#pragma endregion

#pragma region Title
dae::TitleState::TitleState(Game& game)
	: GameState(game)
{
}

void dae::TitleState::OnEnter()
{
    std::cout << "GameState: Entered TitleState\n";
    CreateTitleScreen();
}

void dae::TitleState::OnExit()
{
    InputManager::GetInstance().ClearBindings();
    SceneManager::GetInstance().DestroyAllScenes();
    std::cout << "GameState: TitleScreenExited\n";
}

void dae::TitleState::CreateTitleScreen()
{
    auto& scene{ SceneManager::GetInstance().CreateScene() };
    const auto windowSize{ Renderer::GetInstance().GetWindowSize() };
    const auto windowCentre{ glm::vec2(windowSize.x / 2.f, windowSize.y / 2.f) };

    // --- Fonts ---
    auto mainFont{ dae::ResourceManager::GetInstance().LoadFont("MainFont.ttf", 200) };
    auto subFont{ dae::ResourceManager::GetInstance().LoadFont("SubFont.ttf", 36) };

    // --- Background ---
    auto go{ std::make_unique<dae::GameObject>() };

    // --- Logo ---
    go = std::make_unique<dae::GameObject>();
    go->AddComponent<dae::RenderComponent>();
    go->GetComponent<dae::RenderComponent>().SetTexture("logo.png");
    go->GetComponent<dae::RenderComponent>().SetScale(0.7f);
    const auto dimensions{ go->GetComponent<dae::RenderComponent>().GetDimensions() };

    go->GetComponent<dae::TransformComponent>().SetWorldPosition(windowSize.x - dimensions.x - 20.f, windowSize.y - dimensions.y - 20.f);
    scene.Add(std::move(go));

    // --- Title Background ---
    go = std::make_unique<dae::GameObject>();
    go->AddComponent<dae::RenderComponent>();
    go->AddComponent<dae::TextComponent>("Bomberman", mainFont);
    go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::PALE_BROWN));
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(windowCentre.x + 5.f, windowCentre.y / 2.f + 5.f);
    go->GetComponent<dae::RenderComponent>().SetCentered(true);
    scene.Add(std::move(go));

    // --- Title Foreground ---
    go = std::make_unique<dae::GameObject>();
    go->AddComponent<dae::RenderComponent>();
    go->AddComponent<dae::TextComponent>("Bomberman", mainFont);
    go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::YELLOW));
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(windowCentre.x, windowCentre.y/2.f);
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

    // --- Enter Text ---
    go = std::make_unique<dae::GameObject>();
    go->AddComponent<dae::RenderComponent>();
    go->AddComponent<dae::TextComponent>("> Press A to enter <", subFont);
    go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::RED));
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(windowCentre.x, windowCentre.y * 1.3f);
    go->GetComponent<dae::RenderComponent>().SetCentered(true);
    scene.Add(std::move(go));

    // --- Subtitle Text ---
    go = std::make_unique<dae::GameObject>();
    go->AddComponent<dae::RenderComponent>();
    go->AddComponent<dae::TextComponent>("Made by Dan Runiskovs", subFont);
    go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::GREY));
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(windowCentre.x, windowCentre.y * 1.9f);
    go->GetComponent<dae::RenderComponent>().SetCentered(true);
    scene.Add(std::move(go));

    // --- Bindings ---
    auto& controllerRef = dae::InputManager::GetInstance().AddController(static_cast<uint8_t>(0));

    dae::InputManager::GetInstance().AddBinding(
        std::make_unique<ControllerBinding>(
            controllerRef, ControllerButton::GAMEPAD_A,
            std::make_unique<dae::ChangeStateCommand>(m_Game,
                std::make_unique<dae::MainMenuState>(m_Game)
            ),
            CommandType::OnRelease)
    );
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
#pragma endregion

#pragma region Gamemode Selection
dae::GamemodeSelectionMenuState::GamemodeSelectionMenuState(Game& game)
    :GameState(game)
{
}

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
#pragma endregion

#pragma region PreGame
dae::PreGameState::PreGameState(Game& game, bool isSolo)
    : GameState(game)
    , m_IsSoloGame{ isSolo }
{
}

void dae::PreGameState::OnEnter()
{
    std::cout << "GameState: Entered PreGameState\n";
    std::string text{};
    const auto mode{ static_cast<dae::Bomberman&>(m_Game).GetMatchSession().GetMode() };
    using gm = dae::MatchSession::GameMode;

    switch (mode)
    {
    case gm::Solo:
        text = "Solo";
        break;
    case gm::Coop:
        text = "Co-op";
        break;
    case gm::Pvp:
        text = "PvP";
        break;
    default:
        break;
    }
    std::cout << "GameState: Mode - " << text << "\n";
    CreatePreGameMenu(mode);
}

void dae::PreGameState::OnExit()
{
    InputManager::GetInstance().ClearBindings();
    SceneManager::GetInstance().DestroyAllScenes();
    std::cout << "GameState: PreGameState\n";
}

void dae::PreGameState::Update()
{
    int playersIn{ 0 };
    int playersReady{ 0 };
    // --- Collect Statuses ---
    for (auto& playerStatus : m_Statuses)
    {
        if (playerStatus.isIn) ++playersIn;
        if (playerStatus.isReady) ++playersReady;
    }

    // --- Check if we can start ---
    if (playersIn != 0 && !m_AllAreReady)
    {
        const auto mode{ static_cast<dae::Bomberman&>(m_Game).GetMatchSession().GetMode() };
        using gm = dae::MatchSession::GameMode;
        if (mode != gm::Solo)
        {
            // --- Force more than 1 player --
            if (playersIn <= 1) return;
        }

        // --- All in and ready ---
        if (playersIn == playersReady)
        {
            m_AllAreReady = true;
            m_LobbyStatusText->SetText("Starting Soon!");
            m_LobbyStatusText->SetColor(HexToSDLColor(HCol::WHITE));
            std::cout << "All in and ready!\n";
        }
    }

    // --- Count down till the game starts ---
    if (m_AllAreReady)
    {
        m_TimeTillStart -= Timer::GetInstance().GetElapsed();
        
        // TODO: TEMP
        const float timeLeft{ std::round(m_TimeTillStart * 10) / 10 };
        std::cout << "Time till start: " << std::to_string(timeLeft) << "!\n";

        if (m_TimeTillStart <= 0.f)
        {
            StartGame(playersIn);
        }
    }

}

void dae::PreGameState::CreatePreGameMenu(dae::MatchSession::GameMode gamemode)
{
    CreatePreGameBindings();

    // --- Gamemode-dependant creation ---
    VisualisePreGameMenu(gamemode);
}

void dae::PreGameState::VisualisePreGameMenu(dae::MatchSession::GameMode gamemode)
{
    using gm = dae::MatchSession::GameMode;

    std::string title{};
    switch (gamemode)
    {
    case gm::Solo:
    {
        title = "Solo Game";
        m_ReadyTexts.resize(1);
        break;
    }
    case gm::Coop:
    {
        title = "CO-OP Game";
        m_ReadyTexts.resize(4);
        break;
    }
    case gm::Pvp:
    {
        title = "PvP Game";
        m_ReadyTexts.resize(4);
        break;
    }
    default:
        break;
    }
    const uint8_t playerCount{ static_cast<uint8_t>(m_Statuses.size()) };

    // --- Let's visualise --
    auto& scene{ SceneManager::GetInstance().CreateScene() };
    auto mainFont{ dae::ResourceManager::GetInstance().LoadFont("MainFont.ttf", 150) };
    auto subFont{ dae::ResourceManager::GetInstance().LoadFont("SubFont.ttf", 36) };
    const auto windowSize{ Renderer::GetInstance().GetWindowSize() };
    const auto windowCentre{ glm::vec2(windowSize.x / 2.f, windowSize.y / 2.f) };

    // --- Title Background ---
    auto go{ std::make_unique<dae::GameObject>() };
    go->AddComponent<dae::RenderComponent>();
    go->AddComponent<dae::TextComponent>(title, mainFont);
    go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::PALE_BROWN));
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(windowCentre.x + 5.f, windowCentre.y * .5f + 5.f);
    go->GetComponent<dae::RenderComponent>().SetCentered(true);
    scene.Add(std::move(go));

    // --- Title Foreground ---
    go = std::make_unique<dae::GameObject>();
    go->AddComponent<dae::RenderComponent>();
    go->AddComponent<dae::TextComponent>(title, mainFont);
    go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::YELLOW));
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(windowCentre.x, windowCentre.y * .5f);
    go->GetComponent<dae::RenderComponent>().SetCentered(true);
    scene.Add(std::move(go));

    // --- Ready indicators ---
    for (uint8_t playerIdx{ 0 }; playerIdx < playerCount; ++playerIdx)
    {
        const auto& pos{ dae::Utils::PlayerPosition(playerIdx, playerCount, windowCentre, 150.f) };

        go = std::make_unique<dae::GameObject>();
        go->AddComponent<dae::RenderComponent>();
        m_ReadyTexts[playerIdx] = &go->AddComponent<dae::TextComponent>("------", subFont);
        go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::GREY));
        go->GetComponent<dae::TransformComponent>().SetWorldPosition(pos);
        go->GetComponent<dae::RenderComponent>().SetCentered(true);
        scene.Add(std::move(go));
    }

    // --- Lobby status ---
    go = std::make_unique<dae::GameObject>();
    go->AddComponent<dae::RenderComponent>();
    m_LobbyStatusText = &go->AddComponent<dae::TextComponent>("Press A to join/ready", subFont);
    go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::GREY));
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(windowCentre.x, windowCentre.y * 1.2f);
    go->GetComponent<dae::RenderComponent>().SetCentered(true);
    scene.Add(std::move(go));

    // --- Hnt ---
    go = std::make_unique<dae::GameObject>();
    go->AddComponent<dae::RenderComponent>();
    go->AddComponent<dae::TextComponent>("Ready up only when everyone is in!", subFont);
    go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::GREY));
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(windowCentre.x, windowCentre.y * 1.5f);
    go->GetComponent<dae::RenderComponent>().SetCentered(true);
    scene.Add(std::move(go));
}

void dae::PreGameState::CreatePreGameBindings()
{
    auto& input = dae::InputManager::GetInstance();

    m_Statuses.clear();
    m_Statuses.resize(m_IsSoloGame ? 1 : 4);

    const uint8_t playerCount{ static_cast<uint8_t>(m_Statuses.size()) };

    for (uint8_t playerIdx{ 0 }; playerIdx < playerCount; ++playerIdx)
    {
        auto& controllerRef = input.AddController(playerIdx);

        input.AddBinding(std::make_unique<dae::ControllerBinding>(
            controllerRef,
            dae::ControllerButton::GAMEPAD_A,
            std::make_unique<dae::ExecuteCallbackCommand>(
                [this, playerIdx]()
                {
                    auto& status = m_Statuses[playerIdx];

                    if (!status.isIn)
                    {
                        status.isIn = true;
                        m_ReadyTexts[playerIdx]->SetText("Ready?");

                        status.isReady = false;
                    }
                    else
                    {
                        status.isReady = !status.isReady;
                        m_ReadyTexts[playerIdx]->SetText("Ready!");
                        m_ReadyTexts[playerIdx]->SetColor(HexToSDLColor(dae::Utils::PlayerIdxToHexColor(playerIdx)));
                    }

                    ServiceLocator::GetSoundSystem().PlaySFX(
                        uint8_t(Sounds::Select),
                        10);
                }),
            dae::CommandType::OnRelease));
    }
}

void dae::PreGameState::StartGame(int playerAmount)
{
    static_cast<dae::Bomberman&>(m_Game).GetMatchSession().SetAmoundOfPlayers(static_cast<uint8_t>(playerAmount));

    // --- Reset possible existing score ---
    auto& potentialResult{ static_cast<dae::Bomberman&>(m_Game).GetMatchSession().GetResult() };
    potentialResult.isWin = false;
    potentialResult.score = 0u;
    potentialResult.winnerIdx = 0u; // I guess?
    static_cast<dae::Bomberman&>(m_Game).GetMatchSession().FillAliveMask(playerAmount);

    // TODO: Temp Debug
    const auto mode{ static_cast<dae::Bomberman&>(m_Game).GetMatchSession().GetMode() };
    using gm = dae::MatchSession::GameMode;

    std::string title{};
    switch (mode)
    {
    case gm::Solo:
    {
        title = "Solo Game\n";
        break;
    }
    case gm::Coop:
    {
        title = "CO-OP Game\n";
        break;
    }
    case gm::Pvp:
    {
        title = "PvP Game\n";
        break;
    }
    default:
        break;
    }
    std::cout << "GameState: Starting " << title
        << "With " << std::to_string(static_cast<int>(potentialResult.playerAmount)) << " players\n";

    
    // --- Start the thing ---
    ChangeState(std::make_unique<dae::InGameState>(m_Game));
}

#pragma endregion

#pragma region In Game
dae::InGameState::InGameState(Game& game)
    :GameState(game)
{
}

void dae::InGameState::OnEnter()
{
    auto& session{ static_cast<Bomberman&>(m_Game).GetMatchSession() };
    session.FillAliveMask(session.GetResult().playerAmount);
    const auto gamemode{ session.GetMode() };    
#ifdef _DEBUG
    switch (gamemode)
    {
    case dae::MatchSession::GameMode::Solo:
        std::cout << "GameState: Solo Game Entered \n";
        break;
    case dae::MatchSession::GameMode::Pvp:
        std::cout << "GameState: PvP Game Entered \n";
        break;
    case dae::MatchSession::GameMode::Coop:
        std::cout << "GameState: CO-OP Game Entered \n";
        break;
    default:
        break;
    }
    std::cout << "Creating game...\n";
#endif // _DEBUG
    CreateGame(gamemode);
}

void dae::InGameState::OnExit()
{
    InputManager::GetInstance().ClearBindings();
    SceneManager::GetInstance().DestroyAllScenes();
    Camera::GetInstance().Reset();
    Renderer::GetInstance().SetBackgroundColor(HexToSDLColor(HCol::BLACK));
    CollectResults(static_cast<Bomberman&>(m_Game).GetMatchSession());
    std::cout << "GameState: Game Simulation Exited\n";
}

void dae::InGameState::Update()
{
    AimCamera();

    // --- Update Bombs ---
    for (auto& bomb : m_Bombs)
    {
       auto* pB{ bomb.get() };
       pB->Update();
       if (pB->HasExploded())
       {
           // --- Create Blast ---
           auto& cell{ m_Level.WorldPosToGridCell(pB->GetPosition()) };
           CreateBlast(*m_pScene, cell, pB->GetOwnerColor(), pB->GetBlastRange());
       };
    }

    // --- Update Blasts ---
    for (auto& blast : m_Blasts)
    {
        auto& blastRef{ *blast.get() };
        blastRef.Update();
        for(auto& player : m_Players)
        {
            auto& playerRef{ *player.get() };
            if (!playerRef.IsAlive()) continue;
            if (blastRef.DoesCollide(playerRef.GetBounds()))
            {
                playerRef.Kill();
                continue;
            }
        }
    }

    // --- Update Upgrades ---
    m_Level.ProcessGrid(m_Players);

    // --- Erase exploded bombs ---
    std::erase_if(m_Bombs,
        [](auto& bomb)
        {
            return bomb.get()->HasExploded();
        });
}

void dae::InGameState::TryPlaceBomb(dae::Player& player)
{
    if (!player.TryPlaceBomb()) return;

    auto& cell{ m_Level.WorldPosToGridCell(player.GetWorldPos()) };
    m_Bombs.emplace_back(
        std::make_unique<dae::Bomb>(
            *m_pScene, cell, cell.m_CellSizePx, player.GetBlastRange(), player));    
}

void dae::InGameState::CreateGame(dae::MatchSession::GameMode gamemode)
{
    const auto windowSize{ Renderer::GetInstance().GetWindowSize() };
    //const auto windowCentre{ glm::vec2(windowSize.x / 2.f, windowSize.y / 2.f) };
    // Optional: Set background to Grey
    //Renderer::GetInstance().SetBackgroundColor(HexToSDLColor(HCol::GREY));
    m_pScene = &SceneManager::GetInstance().CreateScene();
    auto go{ std::make_unique<dae::GameObject>() };

#ifdef _DEBUG
    // --- FPS ---
    auto debugFont{ dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 30) };
    go = std::make_unique<dae::GameObject>();
    go->AddComponent<dae::RenderComponent>();
    go->AddComponent<dae::TextComponent>("TEMP", debugFont);
    go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::DEBUG_RED));
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(20.f, 20.f);
    go->AddComponent<dae::FPSComponent>();
    m_pScene->Add(std::move(go));
#endif // _DEBUG

    if (gamemode == dae::MatchSession::GameMode::Pvp)
    {
        CreatePvpLevel(*m_pScene, windowSize);
    }
    else
    {
        CreateNormaLevel(*m_pScene, windowSize);

        // --- Draw Overlay ---
    // --- 1: Black filler --- 
        const glm::vec2 centre{ windowSize.x * 0.5f, 27.f };
        const glm::vec2 dimensions{ windowSize.x, 54.f };
        go = std::make_unique<dae::GameObject>();
        go->AddComponent<dae::RenderComponent>();
        go->GetComponent<dae::RenderComponent>().SetTexture("filler.png");
        go->GetComponent<dae::TransformComponent>().SetWorldPosition(centre.x, centre.y);
        go->GetComponent<dae::RenderComponent>().SetCentered(true);
        go->GetComponent<dae::RenderComponent>().SetDimensions(dimensions.x, dimensions.y);
        go->GetComponent<dae::RenderComponent>().SetStatic(true);
        m_pScene->Add(std::move(go));

        // --- 2. Score: ---
        auto subFont{ dae::ResourceManager::GetInstance().LoadFont("SubFont.ttf", 36) };
        go = std::make_unique<dae::GameObject>();
        go->AddComponent<dae::RenderComponent>().SetCentered(true);
        go->AddComponent<dae::TextComponent>("Score", subFont);
        go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::WHITE));
        go->GetComponent<dae::TransformComponent>().SetWorldPosition(200.f, 27.f);
        m_pScene->Add(std::move(go));

        TextComponent* pText{ nullptr };
        go = std::make_unique<dae::GameObject>();
        go->AddComponent<dae::RenderComponent>().SetCentered(true);
        pText = &go->AddComponent<dae::TextComponent>("0\'000\'000", subFont);
        go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::YELLOW));
        go->GetComponent<dae::TransformComponent>().SetWorldPosition(500.f, 27.f);
        m_pScene->Add(std::move(go));

        m_Level.GetSubject().AddObserver(
            [this, pText](Event e)
            {
                if (e == Event::OnScoreChanged)
                {
                    auto& result{ static_cast<dae::Bomberman&>(m_Game).GetMatchSession().GetResult() };
                    result.score = m_Level.GetCurrentScore();
                    // TODO: visualise score as well
                    std::cout << "Game state: SCORE: " << std::to_string(static_cast<int>(result.score) * 100) << "!\n";
                    if (pText)
                    {
                        auto text{ std::to_string(result.score) };
                        text += "00";

                        std::cout << "Score pre insert: " << text << "\n";

                        // --- Insert missing amount of leading 0 ---
                        for (size_t digitN{ text.length() }; digitN < 7; ++digitN)
                        {
                            text = "0" + text; // Add a leading 0
                        }

                        std::cout << "Score post insert: " << text << "\n";

                        // --- Quick and brutal format ---
                        const std::string newText =
                            std::string(1, text[0]) +
                            "\'" +
                            text[1] +
                            text[2] +
                            text[3] +
                            "\'" +
                            text[4] +
                            text[5] +
                            text[6];
                        pText->SetText(newText);
                    }
                }
            }
        );
    }
    // --- Create Player(s) ---
    const int nPlayers{ static_cast<dae::Bomberman&>(m_Game).GetMatchSession().GetResult().playerAmount };
    CreatePlayers(*m_pScene, nPlayers);
    m_PlayersAlive = nPlayers;
    auto& controllerRef = dae::InputManager::GetInstance().AddController(static_cast<uint8_t>(0));

    dae::InputManager::GetInstance().AddBinding(
        std::make_unique<ControllerBinding>(
            controllerRef, ControllerButton::GAMEPAD_START,

            std::make_unique<dae::ChangeStateCommand>(
                m_Game,
                std::make_unique<dae::GameOverState>(m_Game)
            ),

            CommandType::OnRelease
        )
    );
    

    // --- Fake Results ---
    //CollectResults(static_cast<dae::Bomberman&>(m_Game).GetMatchSession());
}
void dae::InGameState::CreatePvpLevel(Scene& scene, const glm::vec2& windowSize)
{
    // --- Path ---
    auto& rm{ ResourceManager::GetInstance() };
    std::filesystem::path path = rm.GetDataPath();
    path.append("Level/level_pvp.csv");

    // --- Cell Size ---
    const int cellSize{ 54 };
    // --- TopLeft ---
    const glm::vec2 topLeft{
        windowSize.x * 0.35f,
        (windowSize.y - (cellSize * 13.f)) / 2.f
    };
    // --- Load and Init
    m_Level.InitLevelGrid(path, topLeft, cellSize);
    // --- Visualise Base ---
    m_Level.VisualiseBaseGrid(scene);
    // --- Populate Level ---
    dae::LevelGrid::PropAmount pa{
        70, 10, 10, 5
    };
    m_Level.VisualiseProps(scene, pa);

    auto debugFont{ dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 30) };
    auto go{ std::make_unique<dae::GameObject>() };
    go->AddComponent<dae::RenderComponent>();
    go->AddComponent<dae::TextComponent>("Your ad here", debugFont);
    go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::DEBUG_RED));
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(100.f, 100.f);
    m_pScene->Add(std::move(go));

}

void dae::InGameState::CreateNormaLevel(Scene& scene, const glm::vec2& windowSize)
{
    // --- Path ---
    auto& rm{ ResourceManager::GetInstance() };
    std::filesystem::path path = rm.GetDataPath();
    path.append("Level/level_normal.csv");

    // --- Cell Size ---
    const int cellSize{ 54 };
    // --- TopLeft ---
    const glm::vec2 topLeft{
        0.f,
        (windowSize.y - (cellSize * 13.f))
    };

    m_Level.InitLevelGrid(path, topLeft, cellSize);
    // --- Visualise Base ---
    m_Level.VisualiseBaseGrid(scene);
    // --- Populate Level ---
    const bool isSolo{ static_cast<dae::Bomberman&>(m_Game).GetMatchSession().GetResult().playerAmount == 1 };
    dae::LevelGrid::PropAmount pa{
        50, 
        (isSolo) ? 5 : 10, 
        (isSolo) ? 10 : 15,
        5
    };
    m_Level.VisualiseProps(scene, pa);
}

void dae::InGameState::CreatePlayers(Scene& scene, int playerAmount)
{
    m_Players.clear();
    m_Players.reserve(playerAmount);
    for(int playerIdx{ 0 }; playerIdx < playerAmount; ++playerIdx)
    {
        const auto& spawnpoint{ m_Level.GetSpawnpoint(playerIdx) };
        const auto& size{ static_cast<float>(m_Level.At(0, 0).m_CellSizePx) };
        m_Players.emplace_back(std::make_unique<dae::Player>(scene, spawnpoint, glm::vec2{ size*0.8f, size * 0.8f }, size, playerIdx));

        auto& controllerRef = dae::InputManager::GetInstance().AddController(static_cast<uint8_t>(playerIdx));

        // --- Add bomb placement for just inserted player ---
        Player* playerPtr{ m_Players.back().get() };
        dae::InputManager::GetInstance().AddBinding(
            std::make_unique<ControllerBinding>(
                controllerRef,
                ControllerButton::GAMEPAD_A,
                std::make_unique<ExecuteCallbackCommand>(
                    [this, playerPtr]()
                    {
                        TryPlaceBomb(*playerPtr);
                    }),
                CommandType::OnPress)
        );
        playerPtr->GetOnStateChanged().AddObserver(
            [this, playerPtr](Event e)
            {
                if (e == Event::OnDeath)
                {
                    InputManager::GetInstance()
                        .ClearControllerBindings(
                            playerPtr->GetPlayerIndex());
                    --m_PlayersAlive;
                    std::cout << "Player dead! Remaining: " << std::to_string(m_PlayersAlive) << "\n";
                    static_cast<Bomberman&>(m_Game).GetMatchSession().OnPlayerDead(playerPtr->GetPlayerIndex());
                    CheckGameOver();
                }
                else if (e == Event::OnWin)
                {
                    static_cast<Bomberman&>(m_Game).GetMatchSession().GetResult().winnerIdx = playerPtr->GetPlayerIndex();
                    static_cast<Bomberman&>(m_Game).GetMatchSession().GetResult().isWin = true;
                    std::cout << "GG, Players alive: " << std::to_string(m_PlayersAlive) << "\n";
                    CheckGameOver();
                }
            });
        
    }
}

void dae::InGameState::AimCamera()
{
    const auto& session{ static_cast<Bomberman&>(m_Game).GetMatchSession() };
    if (session.GetMode() != MatchSession::GameMode::Pvp)
    {
        glm::vec2 center{};

        for (auto& player : m_Players)
        {
            if (!player.get()->IsAlive()) continue;
            center += player.get()->GetWorldPos();
        }

        center /= m_PlayersAlive;
        const auto levelDimensions{ m_Level.GetWorldDimensions() };
        Camera::GetInstance().Aim(center, levelDimensions.x, levelDimensions.y);
    }
}

void dae::InGameState::CheckGameOver()
{
    auto& session{ static_cast<Bomberman&>(m_Game).GetMatchSession() };
    if (session.GetMode() == dae::MatchSession::GameMode::Pvp)
    {
        if (m_PlayersAlive == 1)
        {
            session.GetResult().isWin = true;

            
            
            // --- End Game ---
            ChangeState(std::make_unique<dae::GameOverState>(m_Game));
        }
    }
    else // solo/coop
    {
        if (session.GetResult().isWin) //marked as win
        {
            
            ChangeState(std::make_unique<dae::GameOverState>(m_Game));
        }
        else
        {
            if (m_PlayersAlive == 0) // see if all are dead
            {
                static_cast<Bomberman&>(m_Game).GetMatchSession().GetResult().isWin = false;
                
                ChangeState(std::make_unique<dae::GameOverState>(m_Game));
            }
        }
    }
}

void dae::InGameState::CreateBlast(Scene& scene, GridCell& origin, dae::Utils::PlayerColors color, uint8_t range)
{
    const int size{ origin.m_CellSizePx };

    // --- Center ---
    m_Blasts.emplace_back(
        std::make_unique<Blast>(
            scene,
            origin,
            size,
            Blast::Orientation::Central,
            color));

    constexpr std::pair<int, int> directions[]
    {
        { 1,  0},
        {-1,  0},
        { 0,  1},
        { 0, -1}
    };

    for (const auto& [dx, dy] : directions)
    {
        const auto orientation
        {
            dx != 0
            ? Blast::Orientation::Horizontal
            : Blast::Orientation::Vertical
        };

        for (uint8_t step{}; step < range; ++step)
        {
            const int x
            {
                static_cast<int>(origin.x) + dx * (step + 1)
            };

            const int y
            {
                static_cast<int>(origin.y) + dy * (step + 1)
            };

            // --- Bounds ---
            if (x < 0 ||
                y < 0 ||
                x >= m_Level.GetWidth() ||
                y >= m_Level.GetHeight())
            {
                break;
            }

            auto& cell
            {
                m_Level.At(
                    static_cast<uint8_t>(x),
                    static_cast<uint8_t>(y))
            };

            // --- Wall blocks ---
            if (cell.type == CellType::Wall)
                break;

            const auto cellTypeCached{ cell.type };

            // --- Spawn blast ---
            m_Blasts.emplace_back(
                std::make_unique<Blast>(
                    scene,
                    cell,
                    size,
                    orientation,
                    color)); 

            // --- Barrel stops ---
            if (cellTypeCached == CellType::Barrel)
                break;
        }
    }
}


void dae::InGameState::CollectResults(dae::MatchSession& session)
{
    static std::random_device rd;
    static std::default_random_engine eng(rd());

    MatchSession::MatchResult result{};
    const auto& gamemode{ session.GetMode() };
    switch (gamemode)
    {
    case dae::MatchSession::GameMode::Solo:
    {
        result.isWin = session.GetResult().isWin;
        result.score = session.GetResult().score;
        break;
    }
    case dae::MatchSession::GameMode::Pvp:
    {
        session.SetLastPlayerAliveAsWinner();
        result.isWin = session.GetResult().isWin;
        result.winnerIdx = session.GetResult().winnerIdx;
        break;
    }
    
    case dae::MatchSession::GameMode::Coop:
    {
        result.isWin = session.GetResult().isWin;
        result.score = session.GetResult().score;
        result.aliveMask = session.GetResult().aliveMask;
        break;
    }
    
    default:
        break;
    }
    static_cast<dae::Bomberman&>(m_Game).GetMatchSession().SetResult(result);
}
#pragma endregion

#pragma region Game over
dae::GameOverState::GameOverState(Game& game)
    :GameState(game)
{
}

void dae::GameOverState::OnEnter()
{
    std::cout << "GameState: Game Over state entered\n";
    CreateGameOver();
}

void dae::GameOverState::OnExit()
{
    InputManager::GetInstance().ClearBindings();
    SceneManager::GetInstance().DestroyAllScenes();
    m_SceneButtons.clear();
    std::cout << "GameState: Game Over state Exited\n";
}

void dae::GameOverState::CreateGameOver()
{
    auto& scene{ SceneManager::GetInstance().CreateScene() };
    const auto& session{ static_cast<dae::Bomberman&>(m_Game).GetMatchSession() };
    const auto windowSize{ Renderer::GetInstance().GetWindowSize() };
    const auto windowCentre{ glm::vec2(windowSize.x / 2.f, windowSize.y / 2.f) };

    // --- Fonts ---
    auto mainFont{ dae::ResourceManager::GetInstance().LoadFont("MainFont.ttf", 150) };
    auto subFont{ dae::ResourceManager::GetInstance().LoadFont("SubFont.ttf", 36) };

    // --- Result ---
#pragma region MODE_DEPENDANT
    auto go{ std::make_unique<dae::GameObject>() };

    switch (session.GetMode())
    {
    case dae::MatchSession::GameMode::Solo:
    {
        if (session.GetResult().isWin)
        {
            // --- Title Background ---
            go = std::make_unique<dae::GameObject>();
            go->AddComponent<dae::RenderComponent>();
            go->AddComponent<dae::TextComponent>("You Win!", mainFont);
            go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::DARK_GREEN));
            go->GetComponent<dae::TransformComponent>().SetWorldPosition(windowCentre.x + 5.f, windowCentre.y * .5f + 5.f);
            go->GetComponent<dae::RenderComponent>().SetCentered(true);
            scene.Add(std::move(go));

            // --- Title Foreground ---
            go = std::make_unique<dae::GameObject>();
            go->AddComponent<dae::RenderComponent>();
            go->AddComponent<dae::TextComponent>("You Win!", mainFont);
            go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::GREEN));
            go->GetComponent<dae::TransformComponent>().SetWorldPosition(windowCentre.x, windowCentre.y * .5f);
            go->GetComponent<dae::RenderComponent>().SetCentered(true);
            scene.Add(std::move(go));
        }
        else
        {
            // --- Title Background ---
            go = std::make_unique<dae::GameObject>();
            go->AddComponent<dae::RenderComponent>();
            go->AddComponent<dae::TextComponent>("Game Over!", mainFont);
            go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::DARK_RED));
            go->GetComponent<dae::TransformComponent>().SetWorldPosition(windowCentre.x + 5.f, windowCentre.y * .5f + 5.f);
            go->GetComponent<dae::RenderComponent>().SetCentered(true);
            scene.Add(std::move(go));

            // --- Title Foreground ---
            go = std::make_unique<dae::GameObject>();
            go->AddComponent<dae::RenderComponent>();
            go->AddComponent<dae::TextComponent>("Game Over!", mainFont);
            go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::RED));
            go->GetComponent<dae::TransformComponent>().SetWorldPosition(windowCentre.x, windowCentre.y * .5f);
            go->GetComponent<dae::RenderComponent>().SetCentered(true);
            scene.Add(std::move(go));
        }
        CreateMenuButtons(scene, false, windowCentre);
        CreateScoreDisplay(scene, windowCentre);
        VisualiseSubtext(scene, windowCentre, session.GetResult().isWin);
        break;
    }
    case dae::MatchSession::GameMode::Pvp:
    {
        // --- To pass on ---
        std::string winnerColor{};
        uint32_t color{};
        uint32_t darkerColor{};

        switch (session.GetResult().winnerIdx)
        {
        case 0:
            winnerColor = "Red";
            color = dae::Utils::Colors::RED;
            darkerColor = dae::Utils::Colors::DARK_RED;
            break;
        case 1:
            winnerColor = "Green";
            color = dae::Utils::Colors::GREEN;
            darkerColor = dae::Utils::Colors::DARK_GREEN;
            break;
        case 2:
            winnerColor = "Blue";
            color = dae::Utils::Colors::BLUE;
            darkerColor = dae::Utils::Colors::DARK_BLUE;
            break;
        case 3:
            winnerColor = "Yellow";
            color = dae::Utils::Colors::YELLOW;
            darkerColor = dae::Utils::Colors::PALE_BROWN;
            break;
        default:
            winnerColor = "Magenta";
            color = dae::Utils::Colors::MAGENTA;
            darkerColor = dae::Utils::Colors::DARK_MAGENTA;
            break;
        }

        const std::string text{ winnerColor + " wins!" };
        // --- Title Background ---
        go = std::make_unique<dae::GameObject>();
        go->AddComponent<dae::RenderComponent>();
        go->AddComponent<dae::TextComponent>(text, mainFont);
        go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(darkerColor));
        go->GetComponent<dae::TransformComponent>().SetWorldPosition(windowCentre.x + 5.f, windowCentre.y * .5f + 5.f);
        go->GetComponent<dae::RenderComponent>().SetCentered(true);
        scene.Add(std::move(go));

        // --- Title Foreground ---
        go = std::make_unique<dae::GameObject>();
        go->AddComponent<dae::RenderComponent>();
        go->AddComponent<dae::TextComponent>(text, mainFont);
        go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(color));
        go->GetComponent<dae::TransformComponent>().SetWorldPosition(windowCentre.x, windowCentre.y * .5f);
        go->GetComponent<dae::RenderComponent>().SetCentered(true);
        scene.Add(std::move(go));

        CreateMenuButtons(scene, true, windowCentre);
        break;
    }
    case dae::MatchSession::GameMode::Coop:
    {
        if (session.GetResult().isWin)
        {
            // --- Title Background ---
            go = std::make_unique<dae::GameObject>();
            go->AddComponent<dae::RenderComponent>();
            go->AddComponent<dae::TextComponent>("You Win!", mainFont);
            go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::DARK_GREEN));
            go->GetComponent<dae::TransformComponent>().SetWorldPosition(windowCentre.x + 5.f, windowCentre.y * .5f + 5.f);
            go->GetComponent<dae::RenderComponent>().SetCentered(true);
            scene.Add(std::move(go));

            // --- Title Foreground ---
            go = std::make_unique<dae::GameObject>();
            go->AddComponent<dae::RenderComponent>();
            go->AddComponent<dae::TextComponent>("You Win!", mainFont);
            go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::GREEN));
            go->GetComponent<dae::TransformComponent>().SetWorldPosition(windowCentre.x, windowCentre.y * .5f);
            go->GetComponent<dae::RenderComponent>().SetCentered(true);
            scene.Add(std::move(go));


        }
        else // LOSS
        {
            // --- Title Background ---
            go = std::make_unique<dae::GameObject>();
            go->AddComponent<dae::RenderComponent>();
            go->AddComponent<dae::TextComponent>("Game Over!", mainFont);
            go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::DARK_RED));
            go->GetComponent<dae::TransformComponent>().SetWorldPosition(windowCentre.x + 5.f, windowCentre.y * .5f + 5.f);
            go->GetComponent<dae::RenderComponent>().SetCentered(true);
            scene.Add(std::move(go));

            // --- Title Foreground ---
            go = std::make_unique<dae::GameObject>();
            go->AddComponent<dae::RenderComponent>();
            go->AddComponent<dae::TextComponent>("Game Over!", mainFont);
            go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::RED));
            go->GetComponent<dae::TransformComponent>().SetWorldPosition(windowCentre.x, windowCentre.y * .5f);
            go->GetComponent<dae::RenderComponent>().SetCentered(true);
            scene.Add(std::move(go));
        }

        const glm::vec2 deadCentre{ windowCentre.x, windowCentre.y * 0.8f };
        CreateDeadPeopleScreen(scene, deadCentre, session);
        CreateMenuButtons(scene, true, windowCentre);
        break;
    }
    default:
        break;
    }
#pragma endregion
    
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
}

void dae::GameOverState::CreateDeadPeopleScreen(Scene& scene, const glm::vec2& centerPos, const dae::MatchSession& session)
{
    assert(session.GetMode() == dae::MatchSession::GameMode::Coop);
    
    // --- Shortcuts ---
    using PlayerColor = dae::Utils::PlayerColors;

    // --- Refs ---
    const auto playerN{ session.GetResult().playerAmount };
    const auto aliveMask{ session.GetResult().aliveMask };
    
    // --- Let's create ---
    auto go{ std::make_unique<dae::GameObject>() };
    auto font{ dae::ResourceManager::GetInstance().LoadFont("Icons.ttf", 50) };
    for (uint8_t playerIdx{ 0 }; playerIdx < playerN; ++playerIdx)
    {
        const auto& pos{ dae::Utils::PlayerPosition(playerIdx, playerN, centerPos, 150.f) };

        // --- Get Player Color ---
        uint32_t hexColor{};
        uint32_t hexColorDarker{};
        const auto playerColor{ static_cast<PlayerColor>(playerIdx) };
        switch (playerColor)
        {
        case PlayerColor::Red:
        {
            hexColor = HCol::RED;
            hexColorDarker = HCol::DARK_RED;
            break;
        }
        case PlayerColor::Green:
        {
            hexColor = HCol::GREEN;
            hexColorDarker = HCol::DARK_GREEN;
            break;
        }
        case PlayerColor::Blue:
        {
            hexColor = HCol::BLUE;
            hexColorDarker = HCol::DARK_BLUE;
            break;
        }
        case PlayerColor::Yellow:
        {
            hexColor = HCol::YELLOW;
            hexColorDarker = HCol::PALE_BROWN;
            break;
        }
        default:
        {
            hexColor = HCol::MAGENTA;
            hexColorDarker = HCol::DARK_MAGENTA;
            break;
        }
        }

        // --- Get Death Display ---
        // B - Heart / r - Skull
        const bool isAlive{ dae::Utils::IsPlayerAlive(aliveMask, playerIdx) };
        const std::string text{ isAlive ? "B" : "r" };
        // --- Status Background ---
        go = std::make_unique<dae::GameObject>();
        go->AddComponent<dae::RenderComponent>();
        go->AddComponent<dae::TextComponent>(text, font);
        go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(hexColorDarker));
        go->GetComponent<dae::TransformComponent>().SetWorldPosition(pos.x, pos.y + 5.f);
        go->GetComponent<dae::RenderComponent>().SetCentered(true);
        scene.Add(std::move(go));

        // --- Status Foreground ---
        go = std::make_unique<dae::GameObject>();
        go->AddComponent<dae::RenderComponent>();
        go->AddComponent<dae::TextComponent>(text, font);
        go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(hexColor));
        go->GetComponent<dae::TransformComponent>().SetWorldPosition(pos.x, pos.y);
        go->GetComponent<dae::RenderComponent>().SetCentered(true);
        scene.Add(std::move(go));
    }
    return;
}

void dae::GameOverState::CreateMenuButtons(Scene& scene, bool isVertical, const glm::vec2& centre)
{
    // --- Buttons ---
    // Clear any previous:
    m_SceneButtons.clear();
    // Add new :
    if (isVertical)
    {
        m_SceneButtons.push_back(Button{ {centre.x, 380.f}, "Restart", scene });
        m_SceneButtons.push_back(Button{ {centre.x, 450.f}, "Main Menu", scene });
        m_SceneButtons.push_back(Button{ {centre.x, 520.f}, "Quit", scene });
    }
    else
    {
        m_SceneButtons.push_back(Button{ {centre.x * 0.3f, centre.y * 1.7f}, "Restart", scene });
        m_SceneButtons.push_back(Button{ {centre.x, centre.y * 1.7f}, "Main Menu", scene });
        m_SceneButtons.push_back(Button{ {centre.x * 1.7f, centre.y * 1.7f}, "Quit", scene });
    }
    CreateMenuBindings(isVertical);

    // Assign Callbacks:
    m_SceneButtons[0].GetSubject().AddObserver(
        [this](Event event)
        {
            switch (event)
            {
            case dae::Event::OnClick:
                std::cout << "Bomberman: Restarting!\n";
                ChangeState(std::make_unique<dae::InGameState>(m_Game));
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
                std::cout << "Bomberman: Launching Main Menu!\n";
                ChangeState(std::make_unique<dae::MainMenuState>(m_Game));
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
                std::cout << "Bomberman: Quit!\n";
                InputManager::GetInstance().QueueExit();
                break;
            default:
                break;
            }
        }
    );
}

void dae::GameOverState::CreateScoreDisplay(Scene& scene, const glm::vec2& centerPos)
{
    auto letterFont{ dae::ResourceManager::GetInstance().LoadFont("SubFont.ttf", 50) };
    const auto interLetterOffset{ 50.f };
    auto globalLetterOffset{ 75.f };
    const auto& session{ static_cast<dae::Bomberman&>(m_Game).GetMatchSession() };
    const auto& score{ session.GetResult().score };
    if (Leaderboard::GetInstance().DoesScoreQualify(score))
    {
        SetMenuButtonsLock(true);

        // --- Make Letter Selector ---
        m_LetterSelectors.push_back(LetterSelector{ {centerPos.x - globalLetterOffset - interLetterOffset, centerPos.y - interLetterOffset}, scene });
        m_LetterSelectors.push_back(LetterSelector{ {centerPos.x - globalLetterOffset, centerPos.y - interLetterOffset}, scene });
        m_LetterSelectors.push_back(LetterSelector{ {centerPos.x - globalLetterOffset + interLetterOffset, centerPos.y - interLetterOffset}, scene });

        // --- Wow ---
        VisualiseScore(scene, centerPos);

        // --- Pretext ---
        globalLetterOffset*=2;
        auto go{ std::make_unique<dae::GameObject>() };
        go->AddComponent<dae::RenderComponent>();
        go->GetComponent<dae::RenderComponent>().SetCentered(true);
        go->AddComponent<dae::TextComponent>("Record for:", letterFont);
        go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::WHITE));
        go->GetComponent<dae::TransformComponent>().SetWorldPosition(centerPos.x * .5f, centerPos.y - interLetterOffset);
        scene.Add(std::move(go));
        
        m_LetterSelectors[0].SetSelected(true);
        CreateScoreBoardBindings();
    }
    else
    {
        auto go{ std::make_unique<dae::GameObject>() };
        go->AddComponent<dae::RenderComponent>();
        go->GetComponent<dae::RenderComponent>().SetCentered(true);
        go->AddComponent<dae::TextComponent>("Score:", letterFont);
        go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::WHITE));
        go->GetComponent<dae::TransformComponent>().SetWorldPosition(centerPos.x * .8f, centerPos.y - interLetterOffset);
        scene.Add(std::move(go));

        // --- Visualise score ---
        VisualiseScore(scene,centerPos);
    }
}

void dae::GameOverState::VisualiseScore(Scene& scene, const glm::vec2& centerPos)
{
    const int scoreHundreds{ static_cast<int>(static_cast<dae::Bomberman&>(m_Game).GetMatchSession().GetResult().score) };
    auto text{ std::to_string(scoreHundreds) };
    text += "00";
    
    std::cout << "Score pre insert: " << text << "\n";

    // --- Insert missing amount of leading 0 ---
    for (size_t digitN{ text.length() }; digitN < 7; ++digitN)
    {
        text = "0" + text; // Add a leading 0
    }

    std::cout << "Score post insert: " << text << "\n";

    // --- Quick and brutal format ---
    const std::string newText =
        std::string(1, text[0]) +
        "\'" +
        text[1] +
        text[2] +
        text[3] +
        "\'" +
        text[4] +
        text[5] +
        text[6];

    // --- Lo and Behold: texture ---
    const auto interLetterOffset{ 50.f };
    auto letterFont{ dae::ResourceManager::GetInstance().LoadFont("SubFont.ttf", 50) };
    auto go{ std::make_unique<dae::GameObject>() };
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(centerPos.x * 1.3f, centerPos.y - interLetterOffset);
    go->AddComponent<dae::RenderComponent>();
    go->GetComponent<dae::RenderComponent>().SetCentered(true);
    go->AddComponent<dae::TextComponent>(newText, letterFont);
    go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::RED));
    scene.Add(std::move(go));

    go = std::make_unique<dae::GameObject>();
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(centerPos.x, centerPos.y - interLetterOffset);
    go->AddComponent<dae::RenderComponent>();
    go->GetComponent<dae::RenderComponent>().SetCentered(true);
    go->AddComponent<dae::TextComponent>(':', letterFont);
    go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::YELLOW));
    scene.Add(std::move(go));

    go = std::make_unique<dae::GameObject>();
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(centerPos.x * 1.7f, centerPos.y - interLetterOffset);
    go->AddComponent<dae::RenderComponent>();
    go->GetComponent<dae::RenderComponent>().SetCentered(true);
    go->AddComponent<dae::TextComponent>("Points!", letterFont);
    go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::YELLOW));
    scene.Add(std::move(go));

}

void dae::GameOverState::VisualiseSubtext(Scene& scene, const glm::vec2& centerPos, bool isWin)
{
    //const uint16_t scoreHundreds{ static_cast<dae::Bomberman&>(m_Game).GetMatchSession().GetResult().score };
    //const int placement{ Leaderboard::GetInstance().GetPotentialPlacement(scoreHundreds) };
    auto letterFont{ dae::ResourceManager::GetInstance().LoadFont("SubFont.ttf", 50) };
    std::string subtext{};
    uint32_t hexColor{ HCol::GREY };
    const auto& result{ static_cast<dae::Bomberman&>(m_Game).GetMatchSession().GetResult() };
    const int placement{ Leaderboard::GetInstance().GetProjectedPlacement(result.score) };
    if (placement <= 10)
    {
        
        switch (placement)
        {
        case 1:
            hexColor = HCol::YELLOW;
            break;
        case 2:
            hexColor = HCol::SILVER;
            break;
        case 3:
            hexColor = HCol::BRONZE;
            break;
        default:
            break;
        }
        subtext = "congratualtions! You are top " + std::to_string(placement);
    }
    else
    {
        subtext = (isWin) ? "You won" : "You Lost!";
    }

    auto go{ std::make_unique<dae::GameObject>() };
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(centerPos.x, centerPos.y * 1.2f);
    go->AddComponent<dae::RenderComponent>();
    go->GetComponent<dae::RenderComponent>().SetCentered(true);
    go->AddComponent<dae::TextComponent>(subtext, letterFont);
    go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(hexColor));
    scene.Add(std::move(go));
}

void dae::GameOverState::SetMenuButtonsLock(bool newLock)
{
    for (auto& button : m_SceneButtons)
    {
        button.SetLock(newLock);
    }
}

void dae::GameOverState::SetLetterSelectorsLock(bool newLock)
{
    for (auto& letter : m_LetterSelectors)
    {
        letter.SetLock(newLock);
    }
}

void dae::GameOverState::CreateScoreBoardBindings()
{
    // --- Bindings ---
    // --- Letter UP/DOWN ---
    auto& controllerRef = dae::InputManager::GetInstance().AddController(static_cast<uint8_t>(0));
    auto& input = InputManager::GetInstance();
    input.AddBinding(std::make_unique<dae::ControllerBinding>(
        controllerRef, dae::ControllerButton::GAMEPAD_DPAD_UP,
        std::make_unique<dae::ExecuteCallbackCommand>([this]() {
            m_LetterSelectors[m_SelectedLetterIdx].RotateLetter(false);
            ServiceLocator::GetSoundSystem().PlaySFX(uint8_t(Sounds::Skip), 10);
            }),
        dae::CommandType::OnPress));
    input.AddBinding(std::make_unique<dae::ControllerBinding>(
        controllerRef, dae::ControllerButton::GAMEPAD_DPAD_DOWN,
        std::make_unique<dae::ExecuteCallbackCommand>([this]() {
            m_LetterSelectors[m_SelectedLetterIdx].RotateLetter(true);
            ServiceLocator::GetSoundSystem().PlaySFX(uint8_t(Sounds::Skip), 10);
            }),
        dae::CommandType::OnPress));
    // --- Select NEXT/PREV ---
    input.AddBinding(std::make_unique<dae::ControllerBinding>(
        controllerRef, dae::ControllerButton::GAMEPAD_A,
        std::make_unique<dae::ExecuteCallbackCommand>([this]() {
            // Select next (confirm current/all)
            char char0{};
            char char1{};
            char char2{};
            std::string text{};
            Leaderboard::Entry entry{};

            switch (m_SelectedLetterIdx)
            {
            case 0:
                m_LetterSelectors[m_SelectedLetterIdx].SetSelected(false);
                ++m_SelectedLetterIdx;
                m_LetterSelectors[m_SelectedLetterIdx].SetSelected(true);
                break;
            case 1:
                m_LetterSelectors[m_SelectedLetterIdx].SetSelected(false);
                ++m_SelectedLetterIdx;
                m_LetterSelectors[m_SelectedLetterIdx].SetSelected(true);
                break;
            case 2:
                m_LetterSelectors[m_SelectedLetterIdx].SetSelected(false);
                ++m_SelectedLetterIdx;
                // --- TEMP: Print out initials
                char0 = m_LetterSelectors[0].GetLetter();
                char1 = m_LetterSelectors[1].GetLetter();
                char2 = m_LetterSelectors[2].GetLetter();

                std::cout << "Record for: " << std::string(1, char0) 
                                            << std::string(1, char1) 
                                            << std::string(1, char2) << "!\n";
                
                entry.initials[0] = char0;
                entry.initials[1] = char1;
                entry.initials[2] = char2;
                entry.scoreHundreds = static_cast<Bomberman&>(m_Game).GetMatchSession().GetResult().score;
                Leaderboard::GetInstance().SaveEntry(entry);

                SetLetterSelectorsLock(true);
                SetMenuButtonsLock(false);
                break;
            default:
                break;
            }
            }),
        dae::CommandType::OnRelease)
    );
    input.AddBinding(std::make_unique<dae::ControllerBinding>(
        controllerRef, dae::ControllerButton::GAMEPAD_B,
        std::make_unique<dae::ExecuteCallbackCommand>([this]() {
            // Go back to prev
            switch (m_SelectedLetterIdx)
            {
            case 0:
                break;
            case 1:
                m_LetterSelectors[m_SelectedLetterIdx].SetSelected(false);
                --m_SelectedLetterIdx;
                m_LetterSelectors[m_SelectedLetterIdx].SetSelected(true);
                break;
            case 2:
                // --- TEMP: Print out initials
                m_LetterSelectors[m_SelectedLetterIdx].SetSelected(false);
                --m_SelectedLetterIdx;
                m_LetterSelectors[m_SelectedLetterIdx].SetSelected(true);
                break;
            default:
                break;
            }
            ServiceLocator::GetSoundSystem().PlaySFX(uint8_t(Sounds::Select), 10);
            }),
        dae::CommandType::OnRelease)
    );
}

#pragma endregion

dae::LeaderboardState::LeaderboardState(Game& game)
    :GameState(game)
{
}

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

    const auto& lb{ Leaderboard::GetInstance() };

    for (size_t i{}; i < lb.GetEntryCount(); ++i)
    {
        const auto& entry{ lb.GetEntries()[i] };

        std::string text
        {
            std::to_string(i + 1)
            + ". "
            + entry.initials[0] + entry.initials[1] + entry.initials[2]
            + "    "
            + std::to_string(entry.scoreHundreds) + "00"
        };

        auto row{ std::make_unique<dae::GameObject>() };

        row->AddComponent<dae::RenderComponent>();
        row->AddComponent<dae::TextComponent>(text, subFont);

        row->GetComponent<dae::TextComponent>()
            .SetColor(HexToSDLColor(HCol::WHITE));

        row->GetComponent<dae::TransformComponent>()
            .SetWorldPosition(
                windowCentre.x,
                startY + rowSpacing * static_cast<float>(i));

        row->GetComponent<dae::RenderComponent>()
            .SetCentered(true);

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
