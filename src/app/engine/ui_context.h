#pragma once

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <array>
#include "motion_data.h"
#include "trajectory_analysis.h"

constexpr float ar = 16.0f / 9.0f;

constexpr int MATCHING_COUNT = 5;
enum MatchingType {
    CDTW,
    WEIGHTDTW,
    WEIGHTDDTW,
    SWDTW,
    LCFM,
    LEXFM
};

struct Matching {
    MatchingType matchingType;
    float *distMatrix;
    std::vector<int> alignmentPath;
    std::array<int, 10> squat_segments;
    int n, m;
    float alignmentCost;

    Matching(MatchingType p_type, float *p_dist_matrix, std::vector<int> &p_alignmentPath,
             std::array<int, 10> &p_squat_segments,
             int p_n, int p_m,
             float p_alignmentCost) : matchingType(p_type), distMatrix(p_dist_matrix), alignmentPath(p_alignmentPath),
                                      squat_segments(p_squat_segments), n(p_n), m(p_m),
                                      alignmentCost(p_alignmentCost) {}

    ~Matching() {
        free(distMatrix);
    }
};

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

    VPContext(ImVec4 clear_color, glm::vec3 center, glm::vec3 camera_pos, glm::vec3 camera_orientation,
              float aspectRatio, float fov, int windowWidth, int windowHeight) {
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

// TODO: nicht immer motionfiles neu parsen
class UIContext {
public:
    UIContext() :
            aligned(false),
            squats(true),
            vsync(true),
            classicDTW(false),
            refPaused(false),
            inpPaused(false),
            c_frame(0) {
        const char *squats_info = R"(..\resources\squats_subject_info.csv)";
        //const char* taichi_info = R"(resources\taichi_subject_info.csv)";
        this->motion_files = motion_info(squats_info);
        //this->taichi_files = motion_info(taichi_info);
        auto *inpCont = new VPContext(ImVec4{0.25f, 0.35f, 0.60f, 0.80f}, glm::vec3{0.4f, 1.0f, 0.0f},
                                      glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3{0.0, 1.0, 0.0}, 1.260f, 45.0f, 1280,
                                      720);
        auto *refCont = new VPContext(ImVec4{0.55f, 0.35f, 0.60f, 0.80f}, glm::vec3{-1.1f, 0.8f, 0.0f},
                                      glm::vec3(2.0f, 2.1f, 2.0f), glm::vec3{0.0, 1.0, 0.0}, 1.039f, 45.0f, 1280,
                                      720);
        this->inputView = inpCont;
        this->refView = refCont;
    }

    bool vsync;
    bool aligned;
    bool refPaused, inpPaused;
    bool squats;
    bool classicDTW;

    VPContext *inputView;
    VPContext *refView;
    std::string input_file;
    std::string reference_file;

    Distances dist_func;
    Trajectoy_analysis *analysis{};
    std::array<Matching *, MATCHING_COUNT> matching_algorithms;
    int c_frame;
    std::vector<motion_data> *motion_files;

    ~UIContext() {
        for (auto matching: matching_algorithms)
            delete matching;
    }
};
