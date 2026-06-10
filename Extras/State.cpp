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

#include <iostream>
#include <cassert>

// TEMP:
#include <random>


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

void dae::GameState::CreateMenuBindings()
{
    // --- Select Default ---
    m_SelectedButtonIndex = 0;
    m_SceneButtons[m_SelectedButtonIndex].SetSelected(true);

    // --- Bindings ---
    auto& controllerRef = dae::InputManager::GetInstance().AddController(static_cast<uint8_t>(0));
    auto& input = InputManager::GetInstance();
    input.AddBinding(std::make_unique<dae::ControllerBinding>(
        controllerRef, dae::ControllerButton::GAMEPAD_DPAD_DOWN,
        std::make_unique<dae::ExecuteCallbackCommand>([this]() {
            RotateButtonSelection(true);
            ServiceLocator::GetSoundSystem().PlaySFX(uint8_t(Sounds::Skip), 10);
            }),
        dae::CommandType::OnPress));
    input.AddBinding(std::make_unique<dae::ControllerBinding>(
        controllerRef, dae::ControllerButton::GAMEPAD_DPAD_UP,
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
        [](Event event)
        {
            switch (event)
            {
            case dae::Event::OnClick:
                std::cout << "Bomberman: Opening Leaderboard!\n";
                std::cout << "Bomberman: WARNING: Not implemented yet!\n";
                // TODO: Open Leaderboard
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
    CreateMenuBindings();
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
    std::cout << "Connected controller amout: " <<
        static_cast<int>(dae::InputManager::GetInstance().GetControllerAmount()) << "\n";
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
                static_cast<dae::Bomberman&>(m_Game).GetMatchSession().SetAmoundOfPlayers(1u);
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
                std::cout << "Bomberman: Launching PvP!\n";
                static_cast<dae::Bomberman&>(m_Game).GetMatchSession().SetMode(dae::MatchSession::GameMode::Pvp);
                ChangeState(std::make_unique<dae::InGameState>(m_Game));
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
                static_cast<dae::Bomberman&>(m_Game).GetMatchSession().SetAmoundOfPlayers(4u);  // TODO: Temp
                ChangeState(std::make_unique<dae::InGameState>(m_Game));
                break;
            default:
                break;
            }

        }
    );
    CreateMenuBindings();
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
    std::cout << "GameState: Game Simulation Exited\n";
}

void dae::InGameState::CreateGame(dae::MatchSession::GameMode gamemode)
{
    std::cout << "Creating Game TEST\n";
    auto& scene{ SceneManager::GetInstance().CreateScene() };
    const auto windowSize{ Renderer::GetInstance().GetWindowSize() };
    const auto windowCentre{ glm::vec2(windowSize.x / 2.f, windowSize.y / 2.f) };

    // --- Fonts ---
    auto mainFont{ dae::ResourceManager::GetInstance().LoadFont("MainFont.ttf", 120) };
    auto subFont{ dae::ResourceManager::GetInstance().LoadFont("SubFont.ttf", 36) };

    // --- TEMP: Title --- 
    std::string title{};
    switch (gamemode)
    {
    case dae::MatchSession::GameMode::Solo:
        title = "Solo Game Simulation";
        break;
    case dae::MatchSession::GameMode::Pvp:
        title = "PvP Game Simulation";
        break;
    case dae::MatchSession::GameMode::Coop:
        title = "CO-OP Game Simulation";
        break;
    default:
        break;
    }

    // --- Title Background ---
    auto go{ std::make_unique<dae::GameObject>() };
    go->AddComponent<dae::RenderComponent>();
    go->AddComponent<dae::TextComponent>(title, mainFont);
    go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::PALE_BROWN));
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(windowCentre.x + 5.f, windowCentre.y * .3f + 5.f);
    go->GetComponent<dae::RenderComponent>().SetCentered(true);
    scene.Add(std::move(go));

    // --- Title Foreground ---
    go = std::make_unique<dae::GameObject>();
    go->AddComponent<dae::RenderComponent>();
    go->AddComponent<dae::TextComponent>(title, mainFont);
    go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::YELLOW));
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(windowCentre.x, windowCentre.y * .3f);
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

    // --- Helper Text ---
    go = std::make_unique<dae::GameObject>();
    go->AddComponent<dae::RenderComponent>();
    go->AddComponent<dae::TextComponent>("> Press A to exit simulation <", subFont);
    go->GetComponent<dae::TextComponent>().SetColor(HexToSDLColor(HCol::RED));
    go->GetComponent<dae::TransformComponent>().SetWorldPosition(windowCentre.x, windowCentre.y);
    go->GetComponent<dae::RenderComponent>().SetCentered(true);
    scene.Add(std::move(go));

    // --- Bindings ---
    // SOLO:
    
    // PVP:

    // CO-OP:

    auto& controllerRef = dae::InputManager::GetInstance().AddController(static_cast<uint8_t>(0));

    dae::InputManager::GetInstance().AddBinding(
        std::make_unique<ControllerBinding>(
            controllerRef, ControllerButton::GAMEPAD_A,

            std::make_unique<dae::ChangeStateCommand>(
                m_Game,
                std::make_unique<dae::GameOverState>(m_Game)
            ),

            CommandType::OnRelease
        )
    );

    // --- Fake Results ---
    FakeResults(gamemode);
}
void dae::InGameState::FakeResults(dae::MatchSession::GameMode gamemode)
{
    static std::random_device rd;
    static std::default_random_engine eng(rd());

    MatchSession::MatchResult result{};

    switch (gamemode)
    {
    case dae::MatchSession::GameMode::Solo:
    {
        // --- WIN? + SCORE ---
        std::bernoulli_distribution winDist{ 0.5 };
        std::uniform_int_distribution<int> scoreDist{ 0, 999'999 };

        result.isWin = winDist(eng);
        result.score = scoreDist(eng);
        break;
    }
    case dae::MatchSession::GameMode::Pvp:
    {
        // --- WINNER COLOR ---
        std::uniform_int_distribution<uint16_t> winnerDist{ 0, 3 };

        result.winnerIdx = static_cast<uint8_t>(winnerDist(eng));
        break;
    }
    case dae::MatchSession::GameMode::Coop:
    {
        // --- WIN? + SCORE + ALIVE MASK ---
        std::bernoulli_distribution winDist{ 0.5 };
        std::uniform_int_distribution<int> scoreDist{ 0, 999'999 };
        std::uniform_int_distribution<uint16_t> maskDist{ 0, 15 }; // 0000xxxx

        result.isWin = winDist(eng);
        result.score = scoreDist(eng);

        // --- Guarantee at least one alive player on win ---
        if (result.isWin)
        {
            std::uniform_int_distribution<uint16_t> aliveDist{ 1, 15 };
            result.aliveMask = static_cast<uint8_t>(aliveDist(eng));
        }
        else
        {
            result.aliveMask = 0;
        }
    }

        break;
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

    // --- Buttons ---
    // Clear any previous:
    m_SceneButtons.clear();
    // Add new :
    m_SceneButtons.push_back(Button{ {windowCentre.x, 380.f}, "Restart", scene });
    m_SceneButtons.push_back(Button{ {windowCentre.x, 450.f}, "Main Menu", scene });
    m_SceneButtons.push_back(Button{ {windowCentre.x, 520.f}, "Quit", scene });
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
    CreateMenuBindings();
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


#pragma endregion