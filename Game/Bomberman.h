#pragma once

#include "Game.h"
#include "GameStateStack.h"
#include "ScoreManager.h"
#include "Achievements.h"
#include "Button.h"

#include <memory>
#include <vector>
#include <cstdint>

namespace dae
{   
    class Bomberman final : public Game
    {
    public:
        Bomberman() = default;
        ~Bomberman() override = default;

        void Init() override;
        void Update() override;
        void Render() override;
        void HandleInput() override;

        // --- GameStates ---
        void CreateTitleScreen();
        void CreateMainMenu();

        // --- Getter for the Game State Stack ---
        GameStateStack& GetGameStateStack() { return m_GameStateStack; }
    private:
        std::unique_ptr<AchievementManager> m_pAchievementManager{ nullptr };
        GameStateStack m_GameStateStack{};

        // --- Buttons ---
        std::vector<dae::Button>m_SceneButtons{};
        uint8_t m_SelectedButtonIndex{ 0 };
        void RotateButtonSelection(bool isNext);
    };
}