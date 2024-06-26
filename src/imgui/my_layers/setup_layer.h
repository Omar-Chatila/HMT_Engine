#ifndef DISTANCEMEASURES_HMT_SETUP_LAYER_H
#define DISTANCEMEASURES_HMT_SETUP_LAYER_H

#include <GL/glew.h>
#include "../layer.h"
#include "../layer_data.h"
#include "util.h"
#include "../image_loader.h"
#include <imgui.h>
#include <vector>

class SetupLayer : public Layer {
public:
    SetupLayer();

    void onRender() override;

private:
    void
    addLineCheckBox(ImDrawList *draw_list, ImVec2 &image_pos, const char *label, int s_x, int s_y);

    std::vector <Joint::Type> selectedJoints;
    int my_image_width;
    int my_image_height;
    GLuint my_image_texture;
};

#endif //DISTANCEMEASURES_HMT_SETUP_LAYER_H
