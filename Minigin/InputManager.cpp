#include "InputManager.h"
#include <SDL3/SDL.h>
#include <assert.h>

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

dae::Controller& dae::InputManager::AddController(uint8_t id)
{
	auto controller{ std::make_unique<Controller>(id) } ;
	Controller& ref{ *controller };
	m_Controllers.emplace_back(std::move(controller));
	return ref;
}

dae::Controller& dae::InputManager::GetController(uint8_t id)
{
	//assert(id <= m_Controllers.size());
	Controller& ref{ *m_Controllers.at(id) };
	return ref;
}

void dae::InputManager::AddBinding(std::unique_ptr<Binding> binding)
{
	m_Bindings.emplace_back(std::move(binding));
}
