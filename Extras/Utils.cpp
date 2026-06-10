#include "Utils.h"
#include <bit>
#include <cassert>

const SDL_Color dae::Utils::HexToSDLColor(uint32_t hex, uint8_t alpha)
{
    return SDL_Color
    {
        static_cast<Uint8>((hex >> 16) & 0xFF),     // R
        static_cast<Uint8>((hex >> 8) & 0xFF),      // G
        static_cast<Uint8>(hex & 0xFF),             // B
        alpha
    };
}

dae::Utils::PlayerColors dae::Utils::MaskToPlayerAlive(uint8_t mask)
{
    return static_cast<PlayerColors>(std::countr_zero(mask));
}

bool dae::Utils::IsPlayerAlive(uint8_t mask, uint8_t index)
{
    assert(index < 4);
    return mask & (1u << index);
}

const glm::vec2 dae::Utils::PlayerPosition(uint8_t playerIdx, uint8_t playerCount, const glm::vec2& centre, float horizontalMargin)
{
    assert(playerCount > 0 && playerCount <= 4);
    assert(playerIdx < playerCount);

    const float totalWidth{ (playerCount - 1) * horizontalMargin };

    const float startX{ centre.x - totalWidth * 0.5f };

    return
    {
        startX + playerIdx * horizontalMargin,
        centre.y
    };
}
