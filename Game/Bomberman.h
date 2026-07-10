#pragma once

#include "Game.h"
#include "GameStateStack.h"
#include "Achievements.h"
#include "MatchSession.h"

#include <memory>

namespace dae
{   
    class Bomberman final : public Game
    {
    public:
        Bomberman() = default;
        ~Bomberman() override = default;

        void Init() override;
        void Update() override;
        void Render() override {};
        void HandleInput() override;

        // --- Getter for the Game State Stack ---
        GameStateStack& GetGameStateStack() override { return m_GameStateStack; }

        // --- Match Session ---
        MatchSession& GetMatchSession() { return m_MatchSession; }
    private:
        std::unique_ptr<AchievementManager> m_pAchievementManager{ nullptr };
        GameStateStack m_GameStateStack{};
        MatchSession m_MatchSession{};
    };
}