#ifndef DISTANCEMEASURESHMT_DATA_H
#define DISTANCEMEASURESHMT_DATA_H

#include <glm/glm.hpp>
#include <utility>
#include <imgui.h>
#include "motion_file_processor.h"
#include "motion_data.h"

enum DTW_Variant {
    CLASSIC,
    WEIGHTED,
    WEIGHTED_DERIVATIVE
};

struct MainLayerContext {
    bool vsync;
    bool aligned;
    bool refPaused, inpPaused;
    bool squats;
    int c_frame;

    std::pair<int, int> mousePos;
    bool autoMode;
    DTW_Variant dtwVariant;

    std::string input_file;
    std::string reference_file;

    MainLayerContext(std::string pInpfile, std::string pReffile) : vsync(true), aligned(false), refPaused(false),
                                                                   inpPaused(false), squats(true), c_frame(0),
                                                                   mousePos({0.0f, 0.0f}), autoMode(true),
                                                                   dtwVariant(CLASSIC), input_file(std::move(pInpfile)),
                                                                   reference_file(std::move(pReffile)) {

    }

    ~MainLayerContext() = default;
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

class Data {
public:
    static Data *instance;

    Data(const Data &) = delete;

    void operator=(const Data &) = delete;

    static void free() {
        delete instance;
    }

    static Data *getInstance(MotionFileProcessor *p_mfp, TrajectoryAnalysisManager *p_bm) {
        if (instance == nullptr)
            instance = new Data(p_mfp, p_bm);
        return instance;
    }

    static Data *getInstance() {
        return instance;
    }

    std::map<DTW_Variant, Algorithm> dtwMap = {
            {CLASSIC,             DTW},
            {WEIGHTED,            WDTW},
            {WEIGHTED_DERIVATIVE, WDDTW}
    };


    MotionFileProcessor *motionFileProcessor;
    TrajectoryAnalysisManager *bestMatch;
    vector<motion_data> *motionInfo;

    int c_ref_frame;
    int c_inp_frame;
    int c_ref_frame_aligned;
    int c_inp_frame_aligned;

    MainLayerContext *mainLayerContext;
    VPContext *refView;
    VPContext *inputView;

private:
    const char *squats_info = R"(..\resources\squats_subject_info.csv)";

    Data(MotionFileProcessor *p_mfp, TrajectoryAnalysisManager *p_bm) : motionFileProcessor(p_mfp), bestMatch(p_bm) {
        c_inp_frame = c_ref_frame = 0;
        c_ref_frame_aligned = c_inp_frame_aligned = 0;
        mainLayerContext = new MainLayerContext(p_bm->inputFile, p_bm->refFile);
        motionInfo = motion_info(squats_info);
        refView = new VPContext(ImVec4{0.55f, 0.35f, 0.60f, 0.80f}, glm::vec3{-1.1f, 0.8f, 0.0f},
                                glm::vec3(2.0f, 2.1f, 2.0f), glm::vec3{0.0, 1.0, 0.0}, 1.039f, 45.0f, 1280,
                                720);
        inputView = new VPContext(ImVec4{0.25f, 0.35f, 0.60f, 0.80f}, glm::vec3{0.4f, 1.0f, 0.0f},
                                  glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3{0.0, 1.0, 0.0}, 1.260f, 45.0f, 1280,
                                  720);
    }

    ~Data() {
        delete inputView;
        delete refView;
        delete mainLayerContext;
    }
};

#endif //DISTANCEMEASURESHMT_DATA_H
