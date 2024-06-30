#pragma once

#include "trajectory_analysis.h"
#include "input_parser.h"
#include "display_requirements.h"
#include "ui_context.h"
#include "util.h"
#include <vector>
#include <array>
#include <string>
#include <tuple>
#include <iostream>

static constexpr int ALGO_COUNT = 9;

enum Algorithm {
    DTW,
    WDTW,
    WDDTW,
    EDR,
    TWED,
    FRECHET,
    FRECHET_QUAT,
    LC_FRECHET,
    LCSS
};

class TrajectoryAnalysisManager {
public:
    TrajectoryAnalysisManager(const std::string &inputFile, const std::string &refFile, UIContext *context);

    ~TrajectoryAnalysisManager();

    void performAnalysis();

    void updateContext();

    float getAlgorithmResult(enum Algorithm algorithm);

    void updateDisplayRequirements();

    UIContext *getContext();

private:
    DR *displayRequ;
    UIContext *context;
    std::string inputFile;
    std::string refFile;
    std::vector<Frame> inputFrames;
    std::vector<Frame> refFrames;
    std::tuple<float, std::vector<int>, float *> alignment;
    std::tuple<float, std::vector<int>, float *> wdtw_alignment;
    std::tuple<float, std::vector<int>, float *> wddtw_alignment;
    std::array<float, ALGO_COUNT> algorithms_results;
    std::array<float, ERROR_COUNT> error_results;
    Input_parser *inputParser;
    Input_parser *refParser;
    Trajectories *inputTrajectories;
    Trajectories *refTrajectories;
    Trajectoy_analysis *analysis;
};
