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
	go->TryGetComponent<dae::RenderComponent>()->SetTexture("background.png");
	go->TryGetComponent<dae::TransformComponent>()->SetPosition(0.f, 0.f);
	scene.Add(std::move(go));

	// --- Logo ---
	go = std::make_unique<dae::GameObject>();
	//go->SetTexture("logo.png");
	//go->SetPosition(358, 180);
	go->AddComponent<dae::RenderComponent>();
	go->TryGetComponent<dae::RenderComponent>()->SetTexture("logo.png");
	go->TryGetComponent<dae::TransformComponent>()->SetPosition(358.f, 180.f);
	scene.Add(std::move(go));

	// --- Title --- 
	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	//auto to = std::make_unique<dae::TextObject>("Programming 4 Assignment", font);
	//to->SetColor({ 255, 255, 0, 255 });
	//to->SetPosition(292, 20);
	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::TextComponent>("Programming 4 Assignment", font);
	go->TryGetComponent<dae::TextComponent>()->SetColor({ 255, 255, 0, 255 });
	go->TryGetComponent<dae::TransformComponent>()->SetPosition(292.f, 20.f);
	scene.Add(std::move(go));

	
	// --- FPS ---
	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::TextComponent>("TEMP", font);
	go->TryGetComponent<dae::TextComponent>()->SetColor({ 255, 0, 0, 255 });
	go->TryGetComponent<dae::TransformComponent>()->SetPosition(20.f, 20.f);
	go->AddComponent<dae::FPSComponent>();
	assert(go->HasComponent(dae::ComponentBase::ComponentID::FPS) && "No FPS component found");
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
