#include <SDL3/SDL.h>
#include <backends/imgui_impl_sdl3.h>
#include "InputManager.h"

bool dae::InputManager::ProcessInput()
{
	// TODO: On events create new commands, push them back into a std::vector<unique_ptr>m_Commands;, after process input call execute on every single command, clear vector, repeat

	// --- Keyboard ---
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_EVENT_QUIT) {
			return false;
		}
		if (e.type == SDL_EVENT_KEY_DOWN) {
			
		}
		if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
			
		}
		// etc...

		//process event for IMGUI
		ImGui_ImplSDL3_ProcessEvent(&e);

	}
	// --- Gamepad --- 

	
	return true;
}
