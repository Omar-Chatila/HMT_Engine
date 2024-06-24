#pragma once

#include "../layer.h"
#include "ui_context.h"
#include <imgui.h>
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

    void changeInputFile(int selected_index);

private:
    UIContext *m_Context;
    std::vector<float> distances;
    float **costM;
    std::vector<float> costs;
    float maxDistance;
    std::vector <ImU32> colors;
    bool showDiagram = false;
    int isCostDeviation = 0;
    int squat_sampleSize;
    int selected_index = -1;
    int alignment_length;

    std::unique_ptr<float[]> values1;
    std::unique_ptr<float[]> values2;
    float s_min, s_max;
    std::vector <std::pair<int, int>> pathCoords;
    std::vector<int> align_path;
};
