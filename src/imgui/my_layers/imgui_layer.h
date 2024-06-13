#pragma once

#include "../layer.h"
#include "ui_context.h"
#include <imgui.h>
#include <tuple>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <limits>
#include <cmath>
#include <Engine.h>


struct Color {
    float r, g, b, a;
};

struct MatrixContext {
    std::vector<int> align_path;
    int n, m;
    float* mat;
    bool isCostDeviation;
};
class ImGuiLayer : public Layer {
public:
    ImGuiLayer(UIContext *context, SharedData *data);

    void UpdateFOVWithScroll();

    // Define the interpolateColor function
    Color interpolateColor(float value, float minVal, float maxVal, const std::vector<Color> &colorStops);

    ImU32 interpolateColor(float value, float minVal, float maxVal, ImU32 colorLow, ImU32 colorHigh);

    void precomputePathDeviation();

    void precomputeCostDeviation();

    void precomputeDeviation(MatrixContext& context, std::vector<float>& distances, std::vector<ImU32>& colors);

    void drawDTWDiagram();

    void showCameraOptions();

    void show_selectionTable();

    void show_DTW_Options();

    void onRender() override;

    void changeInputFile(int selected_index);

private:
    UIContext *m_Context;
    std::vector<float> distances;
    std::vector<ImU32> colors;
    bool showDiagram = false;
    int isCostDeviation = 0;
    int squat_sampleSize;
    int selected_index = -1;
    int alignment_length;
};
