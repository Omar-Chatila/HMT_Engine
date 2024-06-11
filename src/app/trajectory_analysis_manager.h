#pragma once

#include "trajectory_analysis.h"
#include "input_parser.h"
#include "display_requirements.h"
#include "ui_context.h"
#include <vector>
#include <array>
#include <string>
#include <tuple>
#include <iostream>

static constexpr int ALGO_COUNT = 6;

enum Algorithm {
    DTW,
    EDR,
    TWED,
    FRECHET,
    LC_FRECHET,
    LCSS
};

class TrajectoryAnalysisManager {
public:
    TrajectoryAnalysisManager(const std::string& inputFile, const std::string& refFile, UIContext* context);
    ~TrajectoryAnalysisManager();

    void performAnalysis();
    void updateContext();
    float getAlgorithmResult(enum Algorithm algorithm);
    void updateDisplayRequirements();

private:
    DR *displayRequ;
    UIContext* context;
    std::string inputFile;
    std::string refFile;
    std::vector<Frame> inputFrames;
    std::vector<Frame> refFrames;
    std::tuple<float, std::vector<int>, float*> alignment;
    std::array<float, ALGO_COUNT> algorithms_results;
    Input_parser* inputParser;
    Input_parser* refParser;
    Trajectories* inputTrajectories;
    Trajectories* refTrajectories;
    Trajectoy_analysis* analysis;
};
