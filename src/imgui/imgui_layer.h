#pragma once

#include "Layer.h"
#include "ui_context.h"
#include <imgui.h>
#include <tuple>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <limits>
#include <cmath>
#include <Engine.h>


struct Color
{
    float r, g, b, a;
};
class ImGuiLayer : public Layer {
public:
    ImGuiLayer(UIContext &context);

    void UpdateFOVWithScroll();

    // Define the interpolateColor function
    Color interpolateColor(float value, float minVal, float maxVal, const std::vector<Color> &colorStops);

    ImU32 interpolateColor(float value, float minVal, float maxVal, ImU32 colorLow, ImU32 colorHigh);

    void precomputeDistancesAndColors();

    void drawDTWDiagram();

    void showCameraOptions();

    void show_selectionTable();

    void show_DTW_Options();

    void onRender() override;

private:
    UIContext &m_Context;
    std::vector<float> distances;
    std::vector<ImU32> colors;
    bool showDiagram = false;
    int squat_sampleSize;
    int selected_index = -1;
    int alignment_length;
};
