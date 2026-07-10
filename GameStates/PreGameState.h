#pragma once
#include "GameStateBase.h"
#include "MatchSession.h"

namespace dae
{
    class Game;
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
        std::vector<TextComponent*> m_ReadyTexts{};
        TextComponent* m_LobbyStatusText{ nullptr };

        void CreatePreGameMenu(dae::MatchSession::GameMode gamemode);
        void VisualisePreGameMenu(dae::MatchSession::GameMode gamemode);
        void CreatePreGameBindings();
        void StartGame(int playerAmount);
    };
}