#pragma once

#include "layer.h"
#include <imgui/imgui.h>
#include <tuple>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <limits>
#include <cmath>
#include <math.h>
#include <Engine.h>

class ImGuiLayer : public Layer {
public:
    explicit ImGuiLayer(Data *p_data);

    void UpdateFOVWithScroll();

    void drawDTWDiagram();

    void showCameraOptions();

    void show_selectionTable();

    void show_DTW_Options();

    void onRender() override;

    void changeInputFile(int p_selected_index);

private:
    std::vector<ImU32> colors;
    bool showDiagram = false;
    size_t squat_sampleSize;
    int selected_index = -1;
    Data *data;
};
