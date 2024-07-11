#pragma once

#include "layer_data.h"
#include "data/data.h"

struct MatrixContext {
    std::vector<int> align_path;
    int n, m;
    float *mat;
    bool isCostDeviation;
};

class Layer {
public:
    virtual ~Layer() = default;

    virtual void onAttach() {};

    virtual void onDettach() {};

    virtual void onRender() {};


protected:
    void precomputeDeviation(MatrixContext &context, std::vector<float> &p_distances);

    void precomputePathDeviation();

    std::vector<float> distances;
    float **costM{};
    std::unique_ptr<float[]> values1;
    std::unique_ptr<float[]> values2;
    float s_min{}, s_max{};
    std::vector<std::pair<int, int>> pathCoords;
    std::vector<int> align_path;
};