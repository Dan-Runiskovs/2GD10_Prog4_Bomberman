#include "ThrashTheCache.h"

// --- components ---
#include "FPSComponent.h"
#include "RenderComponent.h"
#include "TextComponent.h"
#include "TransformComponent.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "Renderer.h"

// --- imgui ---
#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>
#include <SDL3/SDL.h>

#include <chrono>
#include <algorithm>
#include <cfloat>

struct transform
{
	float matrix[16] = {
		1, 0, 0, 0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
};

void InitImGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
#if __EMSCRIPTEN__
	// For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
	// You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
	io.IniFilename = NULL;
#endif

	SDL_Window* window = dae::Renderer::GetInstance().GetWindow();
	SDL_Renderer* renderer = dae::Renderer::GetInstance().GetSDLRenderer();
	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer3_Init(renderer);
}

ThrashTheCache::~ThrashTheCache()
{
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
}

void ThrashTheCache::Init()
{
	InitImGui();

	// --- Create scene ---
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
	go->AddComponent<dae::TextComponent>("Thrash the Cache", font);
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

void ThrashTheCache::Render()
{
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	ShowEx1();
	ShowEx2();

	ImGui::Render();
	ImGui_ImplSDLRenderer3_RenderDrawData(
		ImGui::GetDrawData(),
		dae::Renderer::GetInstance().GetSDLRenderer()
	);
}

void ThrashTheCache::ShowEx1()
{
	static int samples{ 10 };

	ImGui::Begin("Exercise 1");

	ImGui::SetNextItemWidth(80.0f);
	ImGui::InputInt("##samples", &samples);

	ImGui::SameLine();
	ImGui::Text("# samples");

	ImGui::Spacing();

	if (ImGui::Button("Trash the cache"))
	{
		m_TrashCacheEx1ButtonPressed = true;
		CalculateEx1(samples);
	}

	if (m_TrashCacheEx1ButtonPressed)
	{
		ImGui::Plot("info", *m_Ex1PlotConfig);
	}

	ImGui::End();
}

void ThrashTheCache::ShowEx2()
{
	static int samples = 100;

	ImGui::Begin("Exercise 2");
	ImGui::SetNextItemWidth(180.f);
	ImGui::InputInt("##samples", &samples);

	if (ImGui::Button("GameObject3D"))
	{
		m_TrashCacheEx2ButtonPressed = true;
		CalculateEx2(samples);
	}

	if (m_TrashCacheEx2ButtonPressed)
		ImGui::Plot("ex2", *m_Ex2PlotConfig);

	if (ImGui::Button("GameObject3DAlt"))
	{
		m_TrashCacheEx2AltButtonPressed = true;
		CalculateEx2v2(samples);
	}

	if (m_TrashCacheEx2AltButtonPressed)
		ImGui::Plot("ex2alt", *m_Ex2v2PlotConfig);

	if (m_TrashCacheEx2ButtonPressed && m_TrashCacheEx2AltButtonPressed)
	{
		ImGui::Text("Combined:");
		ImGui::Plot("combined", *m_Ex2CombinedPlotConfig);
	}

	ImGui::End();
}

void ThrashTheCache::CalculateEx1(int nrOfSamples)
{
	*m_Ex1PlotConfig = {};

	constexpr int arraySize{ 10'000'000 };
	std::vector<int> bigArray(arraySize, 1);

	m_LoopInfoEx1.assign(11, 0.f);

	for (int sampleIdx{ 0 }; sampleIdx < nrOfSamples; ++sampleIdx)
	{
		int idx = 0;
		for (int step = 1; step <= 1024; step *= 2)
		{
			const auto start = std::chrono::high_resolution_clock::now();
			for (int i = 0; i < arraySize; i += step)
			{
				bigArray[i] *= 2;
			}
			const auto end = std::chrono::high_resolution_clock::now();

			m_LoopInfoEx1[idx++] += std::chrono::duration<float, std::milli>(end - start).count();
		}
	}

	for (float& v : m_LoopInfoEx1)
	{
		v /= nrOfSamples;
	}

	auto& plot = *m_Ex1PlotConfig;

	plot.values.ys = m_LoopInfoEx1.data();
	plot.values.count = static_cast<int>(m_LoopInfoEx1.size());
	plot.values.ys_count = 1;

	plot.scale.min = 0.f;
	plot.scale.max =
		*std::max_element(m_LoopInfoEx1.begin(), m_LoopInfoEx1.end()) * 1.1f;

	plot.grid_x.show = true;
	plot.grid_y.show = true;
	plot.tooltip.show = true;
	plot.frame_size = ImVec2(200, 120);
	plot.line_thickness = 2.f;
}

void ThrashTheCache::CalculateEx2(int nrOfSamples)
{
	*m_Ex2PlotConfig = {};
	struct GameObject 
	{ 
		transform local;
		int id{}; 
	};

	constexpr int arraySize = 10'000'000;
	std::vector<GameObject> bigArray(arraySize);

	m_LoopInfoEx2.assign(11, 0.f);

	for (int s = 0; s < nrOfSamples; ++s)
	{
		int idx = 0;
		for (int step = 1; step <= 1024; step *= 2)
		{
			auto start = std::chrono::high_resolution_clock::now();
			for (int i = 0; i < arraySize; i += step)
				bigArray[i].id *= 2;
			auto end = std::chrono::high_resolution_clock::now();

			m_LoopInfoEx2[idx++] +=
				std::chrono::duration<float, std::milli>(end - start).count();
		}
	}

	for (float& v : m_LoopInfoEx2)
		v /= nrOfSamples;

	auto& plot = *m_Ex2PlotConfig;

	plot.values.ys = m_LoopInfoEx2.data();
	plot.values.count = static_cast<int>(m_LoopInfoEx2.size());
	plot.values.ys_count = 1;

	plot.scale.min = 0.f;
	plot.scale.max =
		*std::max_element(m_LoopInfoEx2.begin(), m_LoopInfoEx2.end()) * 1.1f;


	plot.grid_x.show = true;
	plot.grid_y.show = true;
	plot.tooltip.show = true;
	plot.frame_size = ImVec2(200, 120);
	plot.line_thickness = 2.f;

	if (m_TrashCacheEx2AltButtonPressed)
		CalculateEx2Combined();
}

void ThrashTheCache::CalculateEx2v2(int nrOfSamples)
{
	*m_Ex2v2PlotConfig = {};
	struct GameObject 
	{ 
		transform* local; 
		int id{}; 
	};

	constexpr int arraySize = 10'000'000;
	std::vector<GameObject> bigArray(arraySize);

	m_LoopInfoEx2v2.assign(11, 0.f);

	for (int s = 0; s < nrOfSamples; ++s)
	{
		int idx = 0;
		for (int step = 1; step <= 1024; step *= 2)
		{
			auto start = std::chrono::high_resolution_clock::now();
			for (int i = 0; i < arraySize; i += step)
				bigArray[i].id *= 2;
			auto end = std::chrono::high_resolution_clock::now();

			m_LoopInfoEx2v2[idx++] +=
				std::chrono::duration<float, std::milli>(end - start).count();
		}
	}

	for (float& v : m_LoopInfoEx2v2)
		v /= nrOfSamples;

	auto& plot = *m_Ex2v2PlotConfig;

	plot.values.ys = m_LoopInfoEx2v2.data();
	plot.values.count = static_cast<int>(m_LoopInfoEx2v2.size());
	plot.values.ys_count = 1;

	plot.scale.min = 0.f;
	plot.scale.max =
		*std::max_element(m_LoopInfoEx2v2.begin(), m_LoopInfoEx2v2.end()) * 1.1f;


	plot.grid_x.show = true;
	plot.grid_y.show = true;
	plot.tooltip.show = true;
	plot.frame_size = ImVec2(200, 120);
	plot.line_thickness = 2.f;

	if (m_TrashCacheEx2ButtonPressed)
		CalculateEx2Combined();
}

void ThrashTheCache::CalculateEx2Combined()
{
	*m_Ex2CombinedPlotConfig = {};

	m_LoopInfoEx2Combined[0] = m_LoopInfoEx2.data();
	m_LoopInfoEx2Combined[1] = m_LoopInfoEx2v2.data();

	auto& plot = *m_Ex2CombinedPlotConfig;

	plot.values.ys_list = m_LoopInfoEx2Combined;
	plot.values.ys_count = 2;
	plot.values.count = static_cast<int>(m_LoopInfoEx2.size());
	plot.values.offset = 0;

	plot.scale.min = 0.f;
	plot.scale.max =
		std::max(
			*std::max_element(m_LoopInfoEx2.begin(), m_LoopInfoEx2.end()),
			*std::max_element(m_LoopInfoEx2v2.begin(), m_LoopInfoEx2v2.end())
		) * 1.1f;

	plot.grid_x.show = true;
	plot.grid_y.show = true;
	plot.tooltip.show = true;
	plot.frame_size = ImVec2(200, 120);
	plot.line_thickness = 2.f;
}
