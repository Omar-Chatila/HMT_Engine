#ifndef TRAJECTORY_ANALYSIS_MANAGER_H
#define TRAJECTORY_ANALYSIS_MANAGER_H

#include "trajectory_analysis.h"
#include "input_parser.h"
#include "ui_context.h"
#include <vector>
#include <string>
#include <tuple>
#include <iostream>

struct DisplayRequirements {
    std::vector<Frame> &ref_frames;
    std::vector<Frame> &inp_frames;
    UIContext *context;
    std::tuple<float, std::vector<int>, float*> &alignment;
};

class TrajectoryAnalysisManager {
public:
    TrajectoryAnalysisManager(const std::string& inputFile, const std::string& refFile, UIContext* context);
    ~TrajectoryAnalysisManager();

    void performAnalysis();
    void updateContext();
    DisplayRequirements displayRequirements();

private:
    UIContext* context;
    std::string inputFile;
    std::string refFile;
    std::vector<Frame> inputFrames;
    std::vector<Frame> refFrames;
    std::tuple<float, std::vector<int>, float*> alignment;
    Input_parser* inputParser;
    Input_parser* refParser;
    Trajectories* inputTrajectories;
    Trajectories* refTrajectories;
    Trajectoy_analysis* analysis;
};


#endif