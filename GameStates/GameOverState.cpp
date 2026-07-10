#include "GameOverState.h"
#include "Utils.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Bomberman.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "InGameState.h"
#include "MainMenuState.h"
#include "Leaderboard.h"
#include "Bindings.h"
#include "CustomCommands.h"
#include "ServiceLocator.h"

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

dae::GameOverState::GameOverState(Game& game)
    :GameState(game)
{}

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
        const std::string text{ isAlive ? "B" : "r" }; // Heart or skull
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
        globalLetterOffset *= 2;
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
        VisualiseScore(scene, centerPos);
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
