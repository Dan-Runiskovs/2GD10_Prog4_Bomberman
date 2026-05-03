#pragma once
#include <memory>
#include <vector>
#include "State.h"

namespace dae
{
	class GameStateStack
	{
    public:
        void PushState(std::unique_ptr<GameState> state);
        void PopState();
        void ChangeState(std::unique_ptr<GameState> state);

        void HandleInput();
        void Update();
        void ProcessPendingChanges();
        void Render() const;

        bool Empty() const;

        GameStateStack() = default;
        ~GameStateStack() = default;
        GameStateStack(const GameStateStack& other) = delete;
        GameStateStack(GameStateStack&& other) = delete;
        GameStateStack& operator=(const GameStateStack& other) = delete;
        GameStateStack& operator=(GameStateStack&& other) = delete;

    private:

        enum class PendingChangeType
        {
            Push,
            Pop,
            Change
        };

        struct PendingChange
        {
            PendingChangeType type;
            std::unique_ptr<GameState> state;
        };

        std::vector<std::unique_ptr<GameState>> m_States{};
        std::vector<PendingChange> m_PendingChanges{};
	};
}