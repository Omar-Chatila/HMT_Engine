#pragma once

#include "layer_data.h"

class Layer {
public:
    virtual ~Layer() = default;

    virtual void onAttach() {};

    virtual void onDettach() {};

    virtual void onRender() {};

protected:
    SharedData *sharedData;
    std::unique_ptr<float[]> values1;
    std::unique_ptr<float[]> values2;
    UIContext *m_Context;
    float s_min{}, s_max{};
    std::vector<std::pair<int, int>> pathCoords;
    std::vector<int> align_path;
};