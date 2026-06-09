#pragma once
#include "Button.h"
#include "Game.h"
#include "MatchSession.h"
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
        void CreateMenuBindings();

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
        void FakeResults(dae::MatchSession::GameMode gamemode);
    };

    class Scene;
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
    };
#pragma endregion
}