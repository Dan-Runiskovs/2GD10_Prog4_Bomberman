#pragma once
#include "Singleton.h"
#include <glm/glm.hpp>

namespace dae
{
    class Camera final : public Singleton<Camera>
    {
    public:
        void Aim(const glm::vec2& target, float levelWidth, float levelHeight);
        void SetViewport(float width, float height);

        void Reset();

        const glm::vec2& GetPosition() const { return m_Position; }

    private:
        glm::vec2 m_Position{};

        float m_ScreenWidth{};
        float m_ScreenHeight{};
    };
}