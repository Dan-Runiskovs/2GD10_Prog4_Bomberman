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
#include "OrbitComponent.h"
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
	go->GetComponent<dae::TextComponent>().SetColor({ 255, 255, 255, 255 });
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

	// --- Empty --- 
	auto empty = std::make_unique<dae::GameObject>(); 
	empty->GetComponent<dae::TransformComponent>().SetWorldPosition(512.f, 288.f);	// centre of the screen 
	// --- Sun ---
	auto sun = std::make_unique<dae::GameObject>(); 
	sun->AddComponent<dae::RenderComponent>();
	sun->AddComponent<dae::TextComponent>("S", font);
	sun->GetComponent<dae::TextComponent>().SetColor({ 255, 255, 0, 255 });
	sun->AddComponent<dae::OrbitComponent>(50.f, float(PI));
	sun->SetParent(empty.get(),false); 
	// --- Planet --- 
	auto planet = std::make_unique<dae::GameObject>(); 
	planet->AddComponent<dae::RenderComponent>(); 
	planet->AddComponent<dae::TextComponent>("P", font);
	planet->GetComponent<dae::TextComponent>().SetColor({ 165, 42, 42, 255 });
	planet->AddComponent<dae::OrbitComponent>(75.f, float(PI / 4));
	planet->SetParent(sun.get(), false); 
	// --- Move our solar system --- 
	scene.Add(std::move(empty)); 
	scene.Add(std::move(sun)); 
	scene.Add(std::move(planet));
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
