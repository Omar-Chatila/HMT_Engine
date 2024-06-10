#pragma once

#include <imgui.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "motion_data.h"
#include "trajectory_analysis.h"

constexpr float ar = 16.0f / 9.0f;

struct VPContext {
    ImVec4 clear_color;
    glm::vec3 center;
    glm::vec3 camera_pos;
    glm::vec3 camera_orientation;

    float aspectRatio;
    float fov;
    float projection_angle;

    int windowWidth;
    int windowHeight;

    VPContext(ImVec4 clear_color, glm::vec3 center, glm::vec3 camera_pos, glm::vec3 camera_orientation, float aspectRatio, float fov, int windowWidth, int windowHeight) {
        this->clear_color = clear_color;
        this->center = center;
        this->camera_pos = camera_pos;
        this->camera_orientation = camera_orientation;
        this->aspectRatio = aspectRatio;
        this->fov = fov;
        this->windowWidth = windowWidth;
        this->windowHeight = windowHeight;
    }
};



class UIContext {
public:
    UIContext() : 
    aligned(false), 
    squats(true), 
    vsync(true),
    c_frame(0){
        const char* squats_info = R"(resources\squats_subject_info.csv)";
        this->motion_files = motion_info(squats_info);
        VPContext *inpCont = new VPContext(ImVec4{0.25f, 0.35f, 0.60f, 0.80f}, glm::vec3{0.4f,1.0f, 0.0f}, glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3{0.0, 1.0, 0.0}, ar, 45.0f, 1280, 720);
        VPContext *refCont = new VPContext(ImVec4{0.55f, 0.35f, 0.60f, 0.80f}, glm::vec3{0.4f,1.0f, 0.0f}, glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3{0.0, 1.0, 0.0}, ar, 45.0f, 1280, 720);
        this->inputView = inpCont;
        this->refView = refCont;
    }

    bool vsync;
    bool aligned;
    bool squats;
    
    VPContext *inputView;
    VPContext *refView;
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
    float* costmatrix;
    float cost;
    int c_frame;
    std::vector<motion_data> *motion_files;

    ~UIContext() {
        free(std::get<0>(*matrix));
    }
};