#pragma once
#include "GameStateBase.h"

namespace dae
{
    class Game;
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
}