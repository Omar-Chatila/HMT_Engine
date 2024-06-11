#ifndef DISTANCEMEASURES_HMT_RESULT_LAYER_H
#define DISTANCEMEASURES_HMT_RESULT_LAYER_H

#include "Layer.h"
#include "layer_data.h"
#include <imgui.h>

class ResultLayer : public Layer {
public:
    ResultLayer(SharedData *data);
    void onRender() override;

private:
};


#endif //DISTANCEMEASURES_HMT_RESULT_LAYER_H
