#ifndef UI_CONTEXT_H
#define UI_CONTEXT_H

#include <imgui.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "motion_data.h"
#include "trajectory_analysis.h"

constexpr float ar = 16.0f / 9.0f;

class UIContext {
public:
    UIContext() : 
    camera_pos({glm::vec3(2.0f, 2.0f, 2.0f)}), 
    aspectRatio(ar),
    clear_color({0.45f, 0.55f, 0.60f, 1.00f}), 
    fov(45.0f), 
    windowHeight(720), 
    windowWidth(1280), 
    camera_orientation(0.0, 1.0, 0.0),
    center({0.4f,1.0f, 0.0f}),
    aligned(false), 
    squats(true), 
    vsync(true),
    c_frame(0){}

    bool vsync;
    bool aligned;
    bool squats;
    ImVec4 clear_color;
    glm::vec3 center;
    glm::vec3 camera_pos;
    glm::vec3 camera_orientation;

    float aspectRatio;
    float fov;
    float projection_angle;

    int windowWidth;
    int windowHeight;

    std::string input_file;
    std::string reference_file;

    int selected_amateur;
    int selected_prof;
    std::vector<std::string> amateur_squats;
    std::vector<std::string> prof_squats;
    std::vector<std::string> amateur_tai;
    std::vector<std::string> prof_tai;

    Distances dist_func;
    Trajectoy_analysis *analysis;
    std::tuple<float*, std::vector<int>, int, int> *matrix;
    float cost;
    int c_frame;
    std::vector<motion_data> *motion_files;

    ~UIContext() {
        free(std::get<0>(*matrix));
    }
};

#endif // UI_CONTEXT_H
