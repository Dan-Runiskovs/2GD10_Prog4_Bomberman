#include "InputManager.h"
//#include "Controller.h"
//#include "Bindings.h"
#include <SDL3/SDL.h>

bool dae::InputManager::ProcessInput()
{
	// --- Default events ---
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_EVENT_QUIT) {
			return false;
		}
		if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
			
		}
	}

	// --- Controller process ---
	for (auto& controller : m_Controllers)
	{
		controller->ProcessInput();
	}

	// --- Execute bindings ---
	for (auto& binding : m_Bindings)
	{
		binding->TryExecute();
	}

	return true;
}

void dae::InputManager::AddController(uint8_t id)
{
	m_Controllers.emplace_back(std::make_unique<Controller>(id));
}

void dae::InputManager::AddBinding(std::unique_ptr<Binding> binding)
{
	m_Bindings.emplace_back(std::move(binding));
}
