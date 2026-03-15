#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <assert.h>
#include <filesystem>

#if _DEBUG && __has_include(<vld.h>)
#include <vld.h>
#endif

#include "Minigin.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "TextComponent.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "FPSComponent.h"
#include "OrbitComponent.h"
#include "Scene.h"

// --- Games --- 
#include "Bomberman.h"
#include "ThrashTheCache.h"
#include "InputTest.h"

namespace fs = std::filesystem;

int main(int, char*[]) {
#if __EMSCRIPTEN__
	fs::path data_location = "";
#else
	fs::path data_location = "./Data/";
	if(!fs::exists(data_location))
		data_location = "../Data/";
#endif
	dae::Minigin engine(data_location, std::make_unique<dae::Bomberman>());
	engine.Run();
    return 0;
}
