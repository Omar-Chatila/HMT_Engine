#ifndef DISTANCEMEASURES_HMT_RESULT_LAYER_H
#define DISTANCEMEASURES_HMT_RESULT_LAYER_H

#include "layer.h"
#include "../layer_data.h"
#include <imgui/imgui.h>
#include <Eigen/Dense>
#include <cmath>
#include <algorithm>
#include <numeric>

class ResultLayer : public Layer {
public:
    explicit ResultLayer(Data *p_data);

    void onRender() override;

    void sortTrajectoryInfos();

private:
    int currentSortIndex;
    Data *data;
};


#endif //DISTANCEMEASURES_HMT_RESULT_LAYER_H
