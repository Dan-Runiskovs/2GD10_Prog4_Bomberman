#pragma once
#include "GameStateBase.h"
#include "MatchSession.h"
#include "Utils.h"
#include "LevelGrid.h"
#include "Player.h"
#include "Bomb.h"
#include "Blast.h"
#include <glm/glm.hpp>
#include <cstdint>
#include <vector>
#include <memory>

namespace dae
{
    class Game;
    class Scene;
    class InGameState final : public GameState
    {
    public:
        explicit InGameState(Game& game);

        void OnEnter() override;
        void OnExit() override;

        void HandleInput() override {};
        void Update() override;
        void Render() const override {};

    private:
        void CreateGame(dae::MatchSession::GameMode gamemode);
        void CreatePvpLevel(Scene& scene, const glm::vec2& windowSize);
        void CreateNormaLevel(Scene& scene, const glm::vec2& windowSize);
        void CreatePlayers(Scene& scene, int playerAmount);
        void AimCamera();
        void CheckGameOver();
        void CreateBlast(Scene& scene, GridCell& origin, dae::Utils::PlayerColors color, uint8_t range);
        void TryPlaceBomb(dae::Player& player);

        void CollectResults(dae::MatchSession& session);

        LevelGrid m_Level{};
        Scene* m_pScene{};
        int m_PlayersAlive{ 0 };
        std::vector<std::unique_ptr<dae::Player>> m_Players{};
        std::vector<std::unique_ptr<dae::Bomb>> m_Bombs{};
        std::vector<std::unique_ptr<dae::Blast>> m_Blasts{};
    };
}