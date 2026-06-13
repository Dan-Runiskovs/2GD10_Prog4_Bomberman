#pragma once
#include <SDL3/SDL.h>

namespace dae
{
	class IICollideable
	{
	public:
		virtual bool DoesCollide(const SDL_FRect& other) = 0;
	};
}