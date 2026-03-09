#include "InputTest.h"

// --- components ---
#include "FPSComponent.h"
#include "RenderComponent.h"
#include "TextComponent.h"
#include "TransformComponent.h"
#include "MovementComponent.h"
#include "Commands.h"

// --- essential ---
#include "ResourceManager.h"
#include "SceneManager.h"
#include "InputManager.h" // <--- i have this...
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

#ifdef _DEBUG
	// --- FPS ---
	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::RenderComponent>();
	go->AddComponent<dae::TextComponent>("TEMP", font);
	go->GetComponent<dae::TextComponent>().SetColor({ 255, 0, 0, 255 });
	go->GetComponent<dae::TransformComponent>().SetWorldPosition(20.f, 20.f);
	go->AddComponent<dae::FPSComponent>();
	scene.Add(std::move(go));
#endif // _DEBUG

	const auto baseSpeed{ 50.f };
	// --- Balloom ---
	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::RenderComponent>();
	go->AddComponent<dae::TextComponent>("B", font);
	go->GetComponent<dae::TextComponent>().SetColor({ 255, 128, 0, 255 });
	go->GetComponent<dae::TransformComponent>().SetWorldPosition(100.f, 100.f);
	go->AddComponent<dae::MovementComponent>(baseSpeed);
	auto& refBalloom = *go;
	dae::InputManager::GetInstance().AddBinding(std::make_unique<dae::KeyboardBinding>(
																				SDL_SCANCODE_D,
																				std::make_unique<dae::MoveCommand>(refBalloom, 1, 0),
																				dae::CommandType::OnHold));
	dae::InputManager::GetInstance().AddBinding(std::make_unique<dae::KeyboardBinding>(
																				SDL_SCANCODE_A,
																				std::make_unique<dae::MoveCommand>(refBalloom, -1, 0),
																				dae::CommandType::OnHold));
	dae::InputManager::GetInstance().AddBinding(std::make_unique<dae::KeyboardBinding>(
																				SDL_SCANCODE_W,
																				std::make_unique<dae::MoveCommand>(refBalloom, 0, -1),
																				dae::CommandType::OnHold));
	dae::InputManager::GetInstance().AddBinding(std::make_unique<dae::KeyboardBinding>(
																				SDL_SCANCODE_S,
																				std::make_unique<dae::MoveCommand>(refBalloom, 0, 1),
																				dae::CommandType::OnHold));
	scene.Add(std::move(go));

	// --- BomberMan ---
	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::RenderComponent>();
	go->AddComponent<dae::TextComponent>("M", font);
	go->GetComponent<dae::TextComponent>().SetColor({ 255, 255, 255, 255 });
	go->GetComponent<dae::TransformComponent>().SetWorldPosition(200.f, 100.f);
	go->AddComponent<dae::MovementComponent>(baseSpeed * 2);
	
	scene.Add(std::move(go));
}

void InputTest::Update()
{
}

void InputTest::Render()
{
}
