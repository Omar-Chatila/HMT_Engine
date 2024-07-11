#ifndef DISTANCEMEASURES_HMT_CLASSIFIER_LAYER_H
#define DISTANCEMEASURES_HMT_CLASSIFIER_LAYER_H

#include "../layer.h"
#include "../layer_data.h"
#include "util.h"
#include <imgui/imgui.h>
#include <imgui/implot.h>
#include <imgui/implot_internal.h>

class ClassifierLayer : public Layer {
public:
    explicit ClassifierLayer(Data *p_data);

    ~ClassifierLayer();

    void onRender() override;

private:
    int currentFrame;
    bool is_continuous;
    Data *data;

    void errorPlot();

    void segmentPlot();

    void segmentPlotUnAligned();
};


#endif //DISTANCEMEASURES_HMT_CLASSIFIER_LAYER_H
