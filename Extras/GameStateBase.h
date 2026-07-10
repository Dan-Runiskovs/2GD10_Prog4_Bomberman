#pragma once
#include <memory>
#include <vector>
#include <cstdint>
#include "State.h"
#include "Button.h"

namespace dae
{
    class Game;
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
        virtual bool IsTranscendent() const { return false; }

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
}
