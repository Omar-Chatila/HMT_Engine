#ifndef DISTANCEMEASURES_HMT_CLASSIFIER_LAYER_H
#define DISTANCEMEASURES_HMT_CLASSIFIER_LAYER_H

#include "../layer.h"
#include "../layer_data.h"
#include <imgui.h>
#include <imgui/implot.h>
#include <imgui/implot_internal.h>

class ClassifierLayer : public Layer{
public:
    ClassifierLayer(SharedData *data);
    ~ClassifierLayer();
    void onRender() override;

private:
    int *currentFrame;
    bool is_continuous;

    void errorPlot();
};


#endif //DISTANCEMEASURES_HMT_CLASSIFIER_LAYER_H
