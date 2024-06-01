#ifndef UI_CONTEXT_H
#define UI_CONTEXT_H

#include <imgui.h>
#include <glm/glm.hpp>
#include <string>
#include "trajectory_analysis.h"

constexpr float ar = 16.0f / 9.0f;

class UIContext {
public:
    UIContext() : 
    camera_pos({glm::vec3(2.0f, 2.0f, 2.0f)}), 
    aspectRatio(ar),
    near(0.1f),
    far(100.0f), 
    clear_color({0.45f, 0.55f, 0.60f, 1.00f}), 
    fov(45.0f), 
    windowHeight(720), 
    windowWidth(1280), 
    center({0.0f,1.0f, 0.0f}) {}

    ImVec4 clear_color;
    glm::vec3 center;
    glm::vec3 camera_pos;
    glm::vec3 camera_orientation;

    float aspectRatio;
    float fov;
    float projection_angle;
    float near, far;

    int windowWidth;
    int windowHeight;

    std::string input_file;
    std::string reference_file;

    Distances dist_func;
    Trajectoy_analysis *analysis;
};

#endif // UI_CONTEXT_H
