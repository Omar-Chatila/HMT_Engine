#ifndef TRAJECTORY_ANALYSIS_MANAGER_H
#define TRAJECTORY_ANALYSIS_MANAGER_H

#include "trajectory_analysis.h"
#include "input_parser.h"
#include "ui_context.h"
#include "renderer.h"
#include "motion_data.h"
#include <vector>
#include <string>
#include <tuple>
#include <iostream>

class TrajectoryAnalysisManager {
public:
    TrajectoryAnalysisManager(const std::string& inputFile, const std::string& refFile, UIContext* context);
    ~TrajectoryAnalysisManager();

    void performAnalysis();
    void updateContext();
    void render(Renderer& renderer, const glm::mat4& projection, const glm::mat4& view, Sphere& sphere, Shader& sphereShader);

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