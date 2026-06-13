#include "InputManager.h"
#include <SDL3/SDL.h>
#include <assert.h>
#include <iostream>

bool dae::InputManager::ProcessInput()
{
	if (m_Exit) return !m_Exit;

	// --- Default events ---
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_EVENT_QUIT) {
			return false;
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
	// --- Try get an existing one ---
	// --- Return existing controller ---
	for (const auto& controller : m_Controllers)
	{
		if (controller && controller.get()->GetID() == id)
		{
			std::cout << "Returned an existing controller with ID: " << static_cast<int>(id) << "\n";
			return *controller;
		}
	}

	auto controller{ std::make_unique<Controller>(id) } ;
	Controller& ref{ *controller };
	m_Controllers.emplace_back(std::move(controller));
	std::cout << "Controller creaded with id: " << static_cast<int>(id) << "\n";
	return ref;
}

dae::Controller& dae::InputManager::GetController(uint8_t id)
{
	Controller& ref{ *m_Controllers.at(id) };
	return ref;
}

uint8_t dae::InputManager::GetControllerAmount()
{
	return m_Controllers.size();
}

void dae::InputManager::AddBinding(std::unique_ptr<Binding> binding)
{
	m_Bindings.emplace_back(std::move(binding));
}

void dae::InputManager::ClearBindings()
{
	m_Bindings.clear();
}

void dae::InputManager::ClearControllerBindings(uint8_t controllerID)
{
	std::erase_if(
		m_Bindings,
		[controllerID](const auto& binding)
		{
			return binding->BelongsToController(
				controllerID);
		});
}
