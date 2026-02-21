#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

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
#include "Scene.h"
#include <assert.h>

#include <filesystem>
namespace fs = std::filesystem;

static void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene();

	// --- Background ---
	auto go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::RenderComponent>();
	go->GetComponent<dae::RenderComponent>().SetTexture("background.png");
	go->GetComponent<dae::TransformComponent>().SetWorldPosition(0.f, 0.f);
	scene.Add(std::move(go));

	// --- Logo ---
	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::RenderComponent>();
	go->GetComponent<dae::RenderComponent>().SetTexture("logo.png");
	go->GetComponent<dae::TransformComponent>().SetWorldPosition(358.f, 180.f);
	scene.Add(std::move(go));

	// --- Title --- 
	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::RenderComponent>();
	go->AddComponent<dae::TextComponent>("Programming 4 Assignment", font);
	go->GetComponent<dae::TextComponent>().SetColor({ 255, 255, 0, 255 });
	go->GetComponent<dae::TransformComponent>().SetWorldPosition(292.f, 20.f);
	scene.Add(std::move(go));

	
	// --- FPS ---
	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::RenderComponent>();
	go->AddComponent<dae::TextComponent>("TEMP", font);
	go->GetComponent<dae::TextComponent>().SetColor({ 255, 0, 0, 255 });
	go->GetComponent<dae::TransformComponent>().SetWorldPosition(20.f, 20.f);
	go->AddComponent<dae::FPSComponent>();
	scene.Add(std::move(go));
}

int main(int, char*[]) {
#if __EMSCRIPTEN__
	fs::path data_location = "";
#else
	fs::path data_location = "./Data/";
	if(!fs::exists(data_location))
		data_location = "../Data/";
#endif
	dae::Minigin engine(data_location);
	engine.Run(load);
    return 0;
}
