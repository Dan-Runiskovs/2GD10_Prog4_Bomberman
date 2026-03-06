#pragma once
#include "Singleton.h"
#include <Xinput.h>

namespace dae
{
	class InputManager final : public Singleton<InputManager>
	{
	public:
		bool ProcessInput();

	private:
		class InputManagerImpl;

	};

}
