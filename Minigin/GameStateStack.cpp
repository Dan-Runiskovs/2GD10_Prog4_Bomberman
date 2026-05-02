#include "GameStateStack.h"
#include "State.h"

void dae::GameStateStack::PushState(std::unique_ptr<GameState> state)
{
    m_PendingChanges.push_back({ PendingChangeType::Push,std::move(state) });
}

void dae::GameStateStack::PopState()
{
	m_PendingChanges.push_back({ PendingChangeType::Pop, nullptr });
}

void dae::GameStateStack::ChangeState(std::unique_ptr<GameState> state)
{
	m_PendingChanges.push_back({ PendingChangeType::Change,std::move(state) });
}

void dae::GameStateStack::HandleInput()
{
    // --- @DEV this intricate structure to process states in reverse order ---
    for (auto stateIt{ m_States.rbegin() }; stateIt != m_States.rend(); ++stateIt)
    {
        (*stateIt)->HandleInput();

        // --- Stop handling input if blocks previous ---
        if (!(*stateIt)->IsTranscendent()) break;
    }
}

void dae::GameStateStack::Update()
{
    // --- @DEV this intricate structure to process states in reverse order ---
    for (auto stateIt{ m_States.rbegin() }; stateIt != m_States.rend(); ++stateIt)
    {
        (*stateIt)->Update();

        // --- Stop updating if blocks previous ---
        if (!(*stateIt)->IsTranscendent()) break;
    }
}

void dae::GameStateStack::Render() const
{
    // --- Early return ---
    if (m_States.empty()) return;

    // --- Get reverse start index ---
    size_t start{ static_cast<int>(m_States.size()) - 1 };

    // --- Find a non-transparent start state to render ---
    while (start > 0 && m_States.at(start)->IsTransparent())
    {
        --start;
    }

    // --- Continue rendering on top of each other ---
    for (size_t stateIndex{ start }; stateIndex < m_States.size(); ++stateIndex)
    {
        m_States[stateIndex]->Render();
    }
}

bool dae::GameStateStack::Empty() const
{
    return m_States.empty();
}

void dae::GameStateStack::ProcessPendingChanges()
{
    // --- Early return ---
    if (m_PendingChanges.empty()) return;
    
    for (auto& change : m_PendingChanges)
    {
        switch (change.type)
        {
        case PendingChangeType::Push:
            // --- Pause the last state ---
            if (!m_States.empty()) m_States.back()->OnPause();

            // --- Make new one perform OnEnter actions ---
            change.state->OnEnter();

            // --- Move it on the stack ---
            m_States.push_back(std::move(change.state));

            break;

        case PendingChangeType::Pop:
            // --- Exit the previous state and pop it ---
            if (!m_States.empty())
            {
                m_States.back()->OnExit();
                m_States.pop_back();
            }

            // --- Resume the  last state ---
            if (!m_States.empty()) m_States.back()->OnResume();

            break;

        case PendingChangeType::Change:
            // --- Exit the previous state and pop it ---
            if (!m_States.empty())
            {
                m_States.back()->OnExit();
                m_States.pop_back();
            }

            // --- Enter and Move the new state ---
            if (change.state)
            {
                change.state->OnEnter();
                m_States.push_back(std::move(change.state));
            }

            break;
        }
    }

    m_PendingChanges.clear();
}
