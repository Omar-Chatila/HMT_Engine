#ifndef DISTANCEMEASURES_HMT_RESULT_LAYER_H
#define DISTANCEMEASURES_HMT_RESULT_LAYER_H

#include "../layer.h"
#include "../layer_data.h"
#include <imgui/imgui.h>

class ResultLayer : public Layer {
public:
    ResultLayer(SharedData *data);
    void onRender() override;
    void sortTrajectoryInfos();

private:
    int currentSortIndex;
};


#endif //DISTANCEMEASURES_HMT_RESULT_LAYER_H
