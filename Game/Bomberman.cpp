#include "Bomberman.h"
#include <cstdint>

// --- components ---
#include "FPSComponent.h"
#include "RenderComponent.h"
#include "TextComponent.h"
#include "TransformComponent.h"
#include "PhysicsComponent.h"
#include "HealthComponent.h"
#include "Commands.h"

// --- essential ---
#include "ResourceManager.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "Renderer.h"

void dae::Bomberman::Init()
{
	// --- Create scene ---
	auto& scene{ dae::SceneManager::GetInstance().CreateScene() };

	CreateDisplay(scene);
	auto& bomberman =			CreateBomberman(scene);
	/* auto& balloom =  */		CreateBalloom(scene);
	/* auto& healthDisplay = */	CreateHealthDisplay(scene, bomberman);
	/* auto& scoreDisplay = */	CreateScoreDisplay(scene);

	

	auto& controllerRef = dae::InputManager::GetInstance().GetController(0);
	auto& input = dae::InputManager::GetInstance();
	input.AddBinding(std::make_unique<dae::KeyboardBinding>(
		SDL_SCANCODE_SPACE,
		std::make_unique<dae::DamageCommand>(bomberman, 1),
		dae::CommandType::OnRelease));
	input.AddBinding(std::make_unique<dae::ControllerBinding>(
		controllerRef,
		dae::ControllerButton::GAMEPAD_A,
		std::make_unique<dae::IncreaseScoreCommand>(200),
		dae::CommandType::OnRelease));
}

void dae::Bomberman::Update()
{
}

void dae::Bomberman::Render()
{
}

void dae::Bomberman::CreateDisplay(Scene& scene)
{
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
	go->AddComponent<dae::TextComponent>("Observers", font);
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

	//helper text
	font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 20);
	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::RenderComponent>();
	go->AddComponent<dae::TextComponent>("Move Balloom with WASD, press SPACE to deal 1 damage", font);
	go->GetComponent<dae::TextComponent>().SetColor({ 255, 0, 0, 255 });
	go->GetComponent<dae::TransformComponent>().SetWorldPosition(20.f, 450.f);
	scene.Add(std::move(go));
	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::RenderComponent>();
	go->AddComponent<dae::TextComponent>("Move Bomberman with DPad, press Gamepad_A to increase score", font);
	go->GetComponent<dae::TextComponent>().SetColor({ 255, 0, 0, 255 });
	go->GetComponent<dae::TransformComponent>().SetWorldPosition(20.f, 500.f);
	scene.Add(std::move(go));
}

dae::GameObject& dae::Bomberman::CreateBomberman(Scene& scene)
{
	constexpr float BASE_SPEED{ 50.f };
	constexpr int SPEED_SCALE{ 2 };
	constexpr float SPEED{ BASE_SPEED * SPEED_SCALE };

	constexpr int SCALE{ 3 };
	constexpr float BASE_SIZE{ 16.f };
	constexpr float SIZE{ BASE_SIZE * SCALE };


	// --- BomberMan ---
	auto go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::RenderComponent>().SetTexture("Bomberman.png");
	go->GetComponent<dae::RenderComponent>().SetDimensions(SIZE, SIZE);
	go->GetComponent<dae::TransformComponent>().SetWorldPosition(200.f, 100.f);
	go->AddComponent<dae::PhysicsComponent>(SPEED);
	
	auto& bomberRef = *go;
	auto& controllerRef = dae::InputManager::GetInstance().AddController(0);
	auto& input = dae::InputManager::GetInstance();
	input.AddBinding(std::make_unique<dae::ControllerBinding>(
		controllerRef,
		dae::ControllerButton::GAMEPAD_DPAD_RIGHT,
		std::make_unique<dae::MoveCommand>(bomberRef, 1, 0),
		dae::CommandType::OnHold));
	input.AddBinding(std::make_unique<dae::ControllerBinding>(
		controllerRef,
		dae::ControllerButton::GAMEPAD_DPAD_LEFT,
		std::make_unique<dae::MoveCommand>(bomberRef, -1, 0),
		dae::CommandType::OnHold));
	input.AddBinding(std::make_unique<dae::ControllerBinding>(
		controllerRef,
		dae::ControllerButton::GAMEPAD_DPAD_UP,
		std::make_unique<dae::MoveCommand>(bomberRef, 0, -1),
		dae::CommandType::OnHold));
	input.AddBinding(std::make_unique<dae::ControllerBinding>(
		controllerRef,
		dae::ControllerButton::GAMEPAD_DPAD_DOWN,
		std::make_unique<dae::MoveCommand>(bomberRef, 0, 1),
		dae::CommandType::OnHold));

	scene.Add(std::move(go));
	return bomberRef;
}

dae::GameObject& dae::Bomberman::CreateBalloom(Scene& scene)
{
	constexpr float BASE_SPEED{ 50.f };
	constexpr int SPEED_SCALE{ 1 };
	constexpr float SPEED{ BASE_SPEED * SPEED_SCALE };

	constexpr int SCALE{ 3 };
	constexpr float BASE_SIZE{ 16.f };
	constexpr float SIZE{ BASE_SIZE * SCALE };

	// --- Balloom ---
	auto go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::RenderComponent>().SetTexture("Balloom.png");
	go->GetComponent<dae::RenderComponent>().SetDimensions(SIZE, SIZE);
	go->GetComponent<dae::TransformComponent>().SetWorldPosition(100.f, 100.f);
	go->AddComponent<dae::PhysicsComponent>(SPEED);

	auto& input = dae::InputManager::GetInstance();
	auto& refBalloom = *go;
	input.AddBinding(std::make_unique<dae::KeyboardBinding>(
		SDL_SCANCODE_D,
		std::make_unique<dae::MoveCommand>(refBalloom, 1, 0),
		dae::CommandType::OnHold));
	input.AddBinding(std::make_unique<dae::KeyboardBinding>(
		SDL_SCANCODE_A,
		std::make_unique<dae::MoveCommand>(refBalloom, -1, 0),
		dae::CommandType::OnHold));
	input.AddBinding(std::make_unique<dae::KeyboardBinding>(
		SDL_SCANCODE_W,
		std::make_unique<dae::MoveCommand>(refBalloom, 0, -1),
		dae::CommandType::OnHold));
	input.AddBinding(std::make_unique<dae::KeyboardBinding>(
		SDL_SCANCODE_S,
		std::make_unique<dae::MoveCommand>(refBalloom, 0, 1),
		dae::CommandType::OnHold));
	scene.Add(std::move(go));
	return refBalloom;
}

dae::TextComponent& dae::Bomberman::CreateHealthDisplay(Scene& scene, GameObject& player)
{
	auto font{ dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 20) };
	auto go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::RenderComponent>();
	go->AddComponent<dae::TextComponent>("Health: ", font);
	go->GetComponent<dae::TextComponent>().SetColor({ 255, 255, 255, 255 });
	go->GetComponent<dae::TransformComponent>().SetWorldPosition(20.f, 300.f);
	auto& textRef = go->GetComponent<TextComponent>();

	const uint8_t maxHealth{ 3 };
	textRef.SetText("Health: " + std::to_string(maxHealth));
	auto& health = player.AddComponent<HealthComponent>(maxHealth);
	health.GetSubject().AddObserver(
		[&](Event event)
		{
			switch (event)
			{
			case dae::Event::OnHealthChanged:
				textRef.SetText("Health: " + std::to_string(health.GetHealth()));
				break;
			case dae::Event::OnDeath:
				textRef.SetText("Player Dead (kind of)");
				break;
			}

		}
	);

	scene.Add(std::move(go));
	return textRef;
}

dae::TextComponent& dae::Bomberman::CreateScoreDisplay(Scene& scene)
{
	auto font{ dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 20) };
	auto go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::RenderComponent>();
	go->AddComponent<dae::TextComponent>("Health: ", font);
	go->GetComponent<dae::TextComponent>().SetColor({ 255, 255, 255, 255 });
	go->GetComponent<dae::TransformComponent>().SetWorldPosition(20.f, 330.f);
	
	auto& textRef = go->GetComponent<TextComponent>();
	auto& scoreRef = ScoreManager::GetInstance();
	textRef.SetText("Score: " + scoreRef.GetFormatedScore());

	scoreRef.GetSubject().AddObserver
	(
		[&](Event event)
		{
			if (event == Event::OnScoreChanged)
			{
				textRef.SetText("Score: " + scoreRef.GetFormatedScore());
			}
		}
	);
	scene.Add(std::move(go));
	return textRef;
}

