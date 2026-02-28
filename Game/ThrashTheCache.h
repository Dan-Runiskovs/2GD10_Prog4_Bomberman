#pragma once
#include "Game.h"
#include "imgui_plot.h"

#include <memory>
#include <vector>

class ThrashTheCache final : public Game
{
public:
    ThrashTheCache() = default;
    ~ThrashTheCache() override;

    void Init() override;
    void Render() override;

private:
    // --- Plot configs ---
    std::unique_ptr<ImGui::PlotConfig> m_Ex1PlotConfig          { std::make_unique<ImGui::PlotConfig>() };
    std::unique_ptr<ImGui::PlotConfig> m_Ex2PlotConfig          { std::make_unique<ImGui::PlotConfig>() };
    std::unique_ptr<ImGui::PlotConfig> m_Ex2v2PlotConfig        { std::make_unique<ImGui::PlotConfig>() };
    std::unique_ptr<ImGui::PlotConfig> m_Ex2CombinedPlotConfig  { std::make_unique<ImGui::PlotConfig>() };

    // --- Data storage ---
    std::vector<float> m_LoopInfoEx1;
    std::vector<float> m_LoopInfoEx2;
    std::vector<float> m_LoopInfoEx2v2;

    // --- Combined plot ---
    const float* m_LoopInfoEx2Combined[2]{};

    // --- UI ---
    bool m_TrashCacheEx1ButtonPressed{ false };
    bool m_TrashCacheEx2ButtonPressed{ false };
    bool m_TrashCacheEx2AltButtonPressed{ false };

    void ShowEx1();
    void ShowEx2();

    // --- Calculations ---
    void CalculateEx1(int nrOfSamples);
    void CalculateEx2(int nrOfSamples);
    void CalculateEx2v2(int nrOfSamples);
    void CalculateEx2Combined();
};