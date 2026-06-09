#pragma once
#include <SDL3/SDL.h>
#include <cstdint>

namespace dae
{
	namespace Utils
	{
		// --- Data ---
		namespace Colors
		{
			constexpr uint32_t YELLOW{ 0xffd900 };
			constexpr uint32_t PALE_BROWN{ 0xc28905 };
			constexpr uint32_t DEBUG_RED{ 0xff0000 };
			constexpr uint32_t RED{ 0xc20505 };
			constexpr uint32_t DARK_RED{ 0x960e07 };
			constexpr uint32_t GREY{ 0x5e5e5e };
			constexpr uint32_t WHITE{ 0xffffff };
		}
		
		enum class Sounds : uint8_t
		{
			Select = 0u,
			Skip = 1u
		};
		
		// --- Functions ---
		const SDL_Color HexToSDLColor(uint32_t hex, uint8_t alpha = 255);
	}
}