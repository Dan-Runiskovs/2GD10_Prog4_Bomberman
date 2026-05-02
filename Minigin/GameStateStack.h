#pragma once

#include <memory>
#include <vector>

namespace dae
{
	class GameState;

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