#pragma once
#include "Game.h"
#include "ScoreManager.h"
#include "Achievements.h"
#include <memory>

namespace dae
{   
    class GameObject;
    class Scene;
    class TextComponent;
    class AchievementManager;
    class Bomberman final : public Game
    {
    public:
        Bomberman() = default;
        ~Bomberman() override = default;

        void Init() override;
        void Update() override;
        void Render() override;

    private:
        void CreateDisplay(Scene& scene);
        GameObject& CreateBomberman(Scene& scene, size_t index, float x, float y);
        GameObject& CreateBalloom(Scene& scene);
        TextComponent& CreateHealthDisplay(Scene& scene, GameObject& player, size_t index, float x, float y);
        TextComponent& CreateScoreDisplay(Scene& scene, ScoreManager& sm, float x, float y);

        ScoreManager m_Score0{ 0 };
        ScoreManager m_Score1{ 1 };

        std::unique_ptr<AchievementManager> m_pAchievementManager{ nullptr };

        //GameObject& CreateHealthDisplay(Scene& scene);
        //GameObject& CreateScoreDisplay(Scene& scene);

        //GameObject& m_Bomberman;
        //GameObject& m_Balloom;
    };
}