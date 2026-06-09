#include "Utils.h"

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
