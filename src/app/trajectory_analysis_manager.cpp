#include "trajectory_analysis_manager.h"

TrajectoryAnalysisManager::TrajectoryAnalysisManager(const std::string& inputFile, const std::string& refFile, UIContext* context)
    : inputFile(inputFile), refFile(refFile), context(context), inputParser(nullptr), refParser(nullptr), inputTrajectories(nullptr), refTrajectories(nullptr), analysis(nullptr) {
    inputParser = new Input_parser(inputFile.c_str());
    inputFrames = inputParser->get_frames();
    inputTrajectories = new Trajectories(inputFrames);
    refParser = new Input_parser(refFile.c_str());
    refFrames = refParser->get_frames();
    refTrajectories = new Trajectories(refFrames);
    analysis = new Trajectoy_analysis(*inputTrajectories, *refTrajectories);
}

TrajectoryAnalysisManager::~TrajectoryAnalysisManager() {
    delete analysis;
    delete refTrajectories;
    delete refParser;
    delete inputTrajectories;
    delete inputParser;
}

void TrajectoryAnalysisManager::performAnalysis() {
    alignment = analysis->perform_DTW(inputTrajectories->get_anglesTrajectories(), refTrajectories->get_anglesTrajectories());
    context->cost = std::get<0>(alignment);
    algorithms_results[DTW] = std::get<0>(alignment);
    //analysis->perform_EDR(Joint::l_hip, EUCLID, 3.0);
}

void TrajectoryAnalysisManager::updateContext() {
    context->reference_file = cropString(refFile).c_str();
    context->input_file = cropString(inputFile).c_str();
    int n = inputTrajectories->get_anglesTrajectories().size();
    int m = refTrajectories->get_anglesTrajectories().size();
    float* matrixx = std::get<2>(alignment);
    auto mat = new std::tuple{matrixx, std::get<1>(alignment), n, m};
    context->matrix = mat;
}

float TrajectoryAnalysisManager::getAlgorithmResult(enum Algorithm algorithm) {
    return algorithms_results[algorithm];
}

void TrajectoryAnalysisManager::updateDisplayRequirements() {
    DisplayRequirements::getI()->setAlignment(this->alignment);
    DisplayRequirements::getI()->setInpFrames(this->inputFrames);
    DisplayRequirements::getI()->setRefFrames(this->refFrames);
    DisplayRequirements::getI()->setContext(this->context);
}