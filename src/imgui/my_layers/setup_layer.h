#ifndef DISTANCEMEASURES_HMT_SETUP_LAYER_H
#define DISTANCEMEASURES_HMT_SETUP_LAYER_H

#include <GL/glew.h>
#include "../layer.h"
#include "../layer_data.h"
#include "util.h"
#include "../image_loader.h"
#include <imgui/imgui.h>
#include <vector>
#include "algo_settings.h"

class SetupLayer : public Layer {
public:
    SetupLayer(SharedData *data);

    void onRender() override;


private:
    static void addCheckBox(ImVec2 &image_pos, const char *label, int s_x, int s_y, bool &val);

    void ShowAlgorithmSettings();

    std::vector<Joint::Type> selectedJoints;

    static int my_image_width;
    static int my_image_height;
    static GLuint my_image_texture;

    static int wdtw_tooltip_width;
    static int wdtw_tooltip_height;
    static GLuint wdtw_tooltip_texture;

    static int wdtw_tooltip_width2;
    static int wdtw_tooltip_height2;
    static GLuint wdtw_tooltip_texture2;
};

#endif //DISTANCEMEASURES_HMT_SETUP_LAYER_H
