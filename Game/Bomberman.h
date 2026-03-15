#pragma once
#include "Game.h"
#include "ScoreManager.h"

namespace dae
{   
    class GameObject;
    class Scene;
    class TextComponent;

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
        GameObject& CreateBomberman(Scene& scene);
        GameObject& CreateBalloom(Scene& scene);
        TextComponent& CreateHealthDisplay(Scene& scene, GameObject& player);
        TextComponent& CreateScoreDisplay(Scene& scene);

        //GameObject& CreateHealthDisplay(Scene& scene);
        //GameObject& CreateScoreDisplay(Scene& scene);

        //GameObject& m_Bomberman;
        //GameObject& m_Balloom;

        //GameObject& m_ScoreTracker;
        //GameObject& m_HealthTracker;
    };
}