#include "trajectory_analysis_manager.h"

TrajectoryAnalysisManager::TrajectoryAnalysisManager(const std::string &inputFile, const std::string &refFile,
                                                     UIContext *context)
        : inputFile(inputFile), refFile(refFile), context(context), inputParser(nullptr), refParser(nullptr),
          inputTrajectories(nullptr), refTrajectories(nullptr), analysis(nullptr) {
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
    alignment = analysis->perform_DTW(inputTrajectories->get_anglesTrajectories(),
                                      refTrajectories->get_anglesTrajectories());
    wdtw_alignment = analysis->perform_WDTW(inputTrajectories->get_anglesTrajectories(),
                                            refTrajectories->get_anglesTrajectories(), 0.005f,
                                            1.0f);
    algorithms_results[DTW] = std::get<0>(alignment);
    algorithms_results[WDTW] = std::get<float>(wdtw_alignment);
    algorithms_results[EDR] = analysis->perform_EDR_Quat(EUCLID, 0.3);
    algorithms_results[TWED] = analysis->perform_TWED_Quat(EUCLID, 0.3f, 1.0f);
    algorithms_results[LCSS] = analysis->perform_LCSS_Quat(EUCLID, 0.3f, 5.0f);
    algorithms_results[FRECHET] = analysis->perform_FRECHET_Pos(EUCLID);
    algorithms_results[FRECHET_QUAT] = analysis->perform_FRECHET_Quat(EUCLID);
}

void TrajectoryAnalysisManager::updateContext() {
    context->reference_file = cropString(refFile);
    context->input_file = cropString(inputFile);
    int n = inputTrajectories->get_anglesTrajectories().size();
    int m = refTrajectories->get_anglesTrajectories().size();
    float *matrixx = std::get<2>(alignment);
    float *wdtw_matrixx = std::get<2>(wdtw_alignment);
    auto mat = new std::tuple{matrixx, std::get<1>(alignment), n, m};
    auto wdtw_mat = new std::tuple{wdtw_matrixx, std::get<1>(wdtw_alignment), n, m};

    // Classic DTW
    auto cdtw_segments = calcSegmentsAligned(std::get<1>(alignment), inputFrames, refFrames);
    auto cdtw_matching = new Matching(CDTW, std::get<2>(alignment), std::get<1>(alignment), cdtw_segments, n, m,
                                      std::get<0>(alignment));
    context->matching_algorithms[CDTW] = cdtw_matching;

    // Weighted DTW
    auto wdtw_segments = calcSegmentsAligned(std::get<1>(wdtw_alignment), inputFrames, refFrames);
    auto wdtw_matching = new Matching(CDTW, std::get<2>(wdtw_alignment), std::get<1>(wdtw_alignment), wdtw_segments, n,
                                      m, std::get<0>(wdtw_alignment));
    context->matching_algorithms[WEIGHTDTW] = wdtw_matching;
}

UIContext *TrajectoryAnalysisManager::getContext() {
    return context;
}

float TrajectoryAnalysisManager::getAlgorithmResult(enum Algorithm algorithm) {
    return algorithms_results[algorithm];
}

void TrajectoryAnalysisManager::updateDisplayRequirements() {
    DR::getI()->setInpFrames(this->inputFrames);
    DR::getI()->setRefFrames(this->refFrames);
    DR::getI()->setContext(this->context);
}