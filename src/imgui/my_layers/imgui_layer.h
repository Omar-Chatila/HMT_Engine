#pragma once

#include "../layer.h"
#include "ui_context.h"
#include <imgui/imgui.h>
#include <tuple>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <limits>
#include <cmath>
#include <math.h>
#include <Engine.h>

struct MatrixContext {
    std::vector<int> align_path;
    int n, m;
    float *mat;
    bool isCostDeviation;
};

class ImGuiLayer : public Layer {
public:
    ImGuiLayer(UIContext *context, SharedData *data);

    void UpdateFOVWithScroll();

    void precomputePathDeviation();

    void precomputeDeviation(MatrixContext &context, std::vector<float> &distances);

    void drawDTWDiagram();

    void showCameraOptions();

    void show_selectionTable();

    void show_DTW_Options();

    void onRender() override;

    void changeInputFile(int p_selected_index);

private:
    std::vector<float> distances;
    float **costM{};
    std::vector<ImU32> colors;
    bool showDiagram = false;
    bool classic_dtw = true;
    size_t squat_sampleSize;
    int selected_index = -1;
};
