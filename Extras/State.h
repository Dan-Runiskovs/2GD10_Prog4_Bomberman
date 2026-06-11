#pragma once
#include "Button.h"
#include "LetterSelector.h"
#include "Game.h"
#include "MatchSession.h"
#include "LevelGrid.h"
#include <vector>
#include <cstdint>


namespace dae
{
#pragma region Base
	class State
	{
    public:
        virtual ~State() = default;

        virtual void OnEnter() {}
        virtual void OnExit() {}

        virtual void OnPause() {}
        virtual void OnResume() {}

        virtual void HandleInput() = 0;
        virtual void Update() = 0;
        virtual void Render() const = 0;
	};
#pragma endregion

#pragma region GameStates
    class GameState : public State
    {
    public:
        explicit GameState(Game& game);
        virtual ~GameState() = default;

        virtual void OnEnter() override {}
        virtual void OnExit() override {}

        virtual void OnPause() override {}
        virtual void OnResume() override {}

        virtual void HandleInput() override = 0;
        virtual void Update() override = 0;
        virtual void Render() const override = 0;

        // --- Disallow input to lower layers ---
        virtual bool IsTransparent() const { return false; }
        // --- Allow input to lower layers ---
        virtual bool IsTranscendent() const{ return false; }

    protected:
        Game& m_Game;

        // --- Buttons ---
        std::vector<dae::Button>m_SceneButtons{};
        uint8_t m_SelectedButtonIndex{ 0 };
        void RotateButtonSelection(bool isNext);
        void CreateMenuBindings(bool isVertical);

        // --- State Changer ---
        void ChangeState(std::unique_ptr<GameState> newState);

    };

    class TitleState final : public GameState
    {
    public:
        explicit TitleState(Game& game);

        void OnEnter() override;
        void OnExit() override;

        void HandleInput() override {};
        void Update() override {};
        void Render() const override {};
    private:
        void CreateTitleScreen();
    };

    class MainMenuState final : public GameState
    {
    public:
        explicit MainMenuState(Game& game);

        void OnEnter() override;
        void OnExit() override;

        void HandleInput() override {};
        void Update() override {};
        void Render() const override {};
    private:
        void CreateMainMenu();
    };

    class GamemodeSelectionMenuState final : public GameState
    {
    public:
        explicit GamemodeSelectionMenuState(Game& game);

        void OnEnter() override;
        void OnExit() override;

        void HandleInput() override {};
        void Update() override {};
        void Render() const override {};
    private:
        void CreateGamemodeSelection();
    };

    class Scene;
    class TextComponent;
    class PreGameState final : public GameState
    {
    public:
        struct PlayerStatus
        {
            bool isIn{ false };
            bool isReady{ false };
        };

        explicit PreGameState(Game& game, bool isSolo);

        void OnEnter() override;
        void OnExit() override;

        void HandleInput() override {};
        void Update() override;
        void Render() const override {};
    private:
        bool m_IsSoloGame{ false };
        bool m_AllAreReady{ false };
        float m_TimeTillStart{ 2.f };
        std::vector<PlayerStatus> m_Statuses{};
        std::vector<dae::TextComponent*> m_ReadyTexts{};
        TextComponent* m_LobbyStatusText{ nullptr };

        void CreatePreGameMenu(dae::MatchSession::GameMode gamemode);
        void VisualisePreGameMenu(dae::MatchSession::GameMode gamemode);
        void CreatePreGameBindings();
        void StartGame(int playerAmount);
    };

    class InGameState final : public GameState
    {
    public:
        explicit InGameState(Game& game);

        void OnEnter() override;
        void OnExit() override;

        void HandleInput() override {};
        void Update() override {};
        void Render() const override {};
    private:
        void CreateGame(dae::MatchSession::GameMode gamemode);
        void CreatePvpLevel(Scene& scene, const glm::vec2& windowCentre);
        //void CreateNormaLevel(Scene& scene, const glm::vec2& windowCentre);

        void FakeResults(dae::MatchSession::GameMode gamemode);

        LevelGrid m_Level{};
    };
    
    class GameOverState final : public GameState
    {
    public:
        explicit GameOverState(Game& game);

        void OnEnter() override;
        void OnExit() override;

        void HandleInput() override {};
        void Update() override {};
        void Render() const override {};
    private:
        void CreateGameOver();

        void CreateDeadPeopleScreen(Scene& scene, const glm::vec2& centerPos, const dae::MatchSession& session);
        void CreateMenuButtons(Scene& scene, bool isVertical, const glm::vec2& centre);
        void CreateScoreDisplay(Scene& scene, const glm::vec2& centerPos, bool isWin);
        void VisualiseScore(Scene& scene, const glm::vec2& centerPos);
        void VisualiseSubtext(Scene& scene, const glm::vec2& centerPos, bool isWin);
        void SetMenuButtonsLock(bool newLock);
        void SetLetterSelectorsLock(bool newLock);
        void CreateScoreBoardBindings(); // Only called in case of WIN

        size_t m_SelectedLetterIdx{ 0 };
        std::vector<dae::LetterSelector> m_LetterSelectors{};
    };
#pragma endregion
}