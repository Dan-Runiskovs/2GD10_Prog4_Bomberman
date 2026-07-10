#include "InGameState.h"
#include "Bomberman.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Camera.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Bindings.h"
#include "CustomCommands.h"
#include "GameOverState.h"

// --- Components ---
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "TextComponent.h"
#include "FPSComponent.h"

// --- Others ---
#include <iostream>
#include <random>

using dae::Utils::HexToSDLColor;
namespace HCol = dae::Utils::Colors;
using dae::Utils::Sounds;

dae::InGameState::InGameState(Game& game)
    :GameState(game)
{}

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
        for (auto& player : m_Players)
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
    for (int playerIdx{ 0 }; playerIdx < playerAmount; ++playerIdx)
    {
        const auto& spawnpoint{ m_Level.GetSpawnpoint(playerIdx) };
        const auto& size{ static_cast<float>(m_Level.At(0, 0).m_CellSizePx) };
        m_Players.emplace_back(std::make_unique<dae::Player>(scene, spawnpoint, glm::vec2{ size * 0.8f, size * 0.8f }, size, playerIdx));

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
            if (cellTypeCached == CellType::Barrel) break;
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
