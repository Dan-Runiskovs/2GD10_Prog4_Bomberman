#include "PreGameState.h"
#include "Bomberman.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Utils.h"
#include "Timer.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "GameObject.h"
#include "Bindings.h"
#include "CustomCommands.h"
#include "ServiceLocator.h"
#include "InGameState.h"

// --- Components ---
#include "TextComponent.h"
#include "RenderComponent.h"
#include "TransformComponent.h"

// --- Other ---
#include <iostream>

using dae::Utils::HexToSDLColor;
namespace HCol = dae::Utils::Colors;
using dae::Utils::Sounds;

dae::PreGameState::PreGameState(Game& game, bool isSolo)
    : GameState(game)
    , m_IsSoloGame{ isSolo }
{}

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