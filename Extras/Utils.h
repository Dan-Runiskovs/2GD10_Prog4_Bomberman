#pragma once
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
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
			constexpr uint32_t GREEN{ 0x0bbf11 };
			constexpr uint32_t DARK_GREEN{ 0x1b731e };
			constexpr uint32_t GREY{ 0x5e5e5e };
			constexpr uint32_t WHITE{ 0xffffff };
			constexpr uint32_t BLUE{ 0x1a69e3 }; // nice
			constexpr uint32_t DARK_BLUE{ 0x2e3f9e };
			constexpr uint32_t MAGENTA{ 0xd51ae3 };
			constexpr uint32_t DARK_MAGENTA{ 0x962e9e };
			constexpr uint32_t BLACK{ 0x000000 };
			constexpr uint32_t SILVER{ 0xcfcec9 };
			constexpr uint32_t BRONZE{ 0xb07616 };
		}
		enum class PlayerColors : uint8_t
		{
			Red = 0u,
			Green = 1u,
			Blue = 2u,
			Yellow = 3u,
			Magenta = 4u
		};
		
		enum class Sounds : uint8_t
		{
			Select = 0u,
			Skip = 1u
		};
		
		// --- Functions ---
		const SDL_Color HexToSDLColor(uint32_t hex, uint8_t alpha = 255);
		PlayerColors MaskToPlayerAlive(uint8_t mask);
		bool IsPlayerAlive(uint8_t mask, uint8_t index);
		const glm::vec2 PlayerPosition(uint8_t playerIdx, uint8_t playerCount, const glm::vec2& centre, float horizontalMargin);
	}
}