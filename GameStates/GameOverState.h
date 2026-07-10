#pragma once
#include "GameStateBase.h"
#include "MatchSession.h"
#include "LetterSelector.h"
#include <glm/glm.hpp>
#include <vector>

namespace dae
{
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
        void CreateMenuButtons(Scene& scene, bool isVertical, const glm::vec2& centre);
        void CreateScoreDisplay(Scene& scene, const glm::vec2& centerPos);
        void VisualiseScore(Scene& scene, const glm::vec2& centerPos);
        void VisualiseSubtext(Scene& scene, const glm::vec2& centerPos, bool isWin);
        void SetMenuButtonsLock(bool newLock);
        void SetLetterSelectorsLock(bool newLock);
        void CreateScoreBoardBindings(); // Only called in case of WIN


        size_t m_SelectedLetterIdx{ 0 };
        std::vector<dae::LetterSelector> m_LetterSelectors{};
    };
}