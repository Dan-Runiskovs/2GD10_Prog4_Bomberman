#include "InputTest.h"

// --- components ---
#include "FPSComponent.h"
#include "RenderComponent.h"
#include "TextComponent.h"
#include "TransformComponent.h"

// --- essential ---
#include "ResourceManager.h"
#include "SceneManager.h"
#include "Renderer.h"

void InputTest::Init()
{
	// --- Create scene ---
	auto& scene{ dae::SceneManager::GetInstance().CreateScene() };

	// --- Background ---
	auto go{ std::make_unique<dae::GameObject>() };
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
	auto font{ dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36) };
	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::RenderComponent>();
	go->AddComponent<dae::TextComponent>("Command and pImpl", font);
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
}

void InputTest::Render()
{
}
