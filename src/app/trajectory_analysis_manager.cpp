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
    AlgoSettings &settings = AlgoSettings::getInstance();
    alignment = analysis->perform_standardized_DTW(
            inputTrajectories->get_anglesTrajectories(),
            refTrajectories->get_anglesTrajectories(), EUCLID);
    wdtw_alignment = analysis->perform_WDTW(inputTrajectories->get_anglesTrajectories(),
                                            refTrajectories->get_anglesTrajectories(), settings.wdtw_g,
                                            settings.wdtw_w_max, EUCLID);
    wddtw_alignment = analysis->perform_WDDTW(inputTrajectories->get_anglesTrajectories(),
                                              refTrajectories->get_anglesTrajectories(), settings.wddtw_g,
                                              settings.wddtw_w_max, EUCLID);

    algorithms_results[DTW] = std::get<0>(alignment);
    algorithms_results[WDTW] = std::get<float>(wdtw_alignment);
    algorithms_results[EDR] = analysis->perform_EDR_Quat(settings.edr_distance, settings.edr_epsilon);
    algorithms_results[TWED] = analysis->perform_TWED_Quat(settings.twed_distance, settings.twed_nu,
                                                           settings.twed_lambda);
    algorithms_results[LCSS] = analysis->perform_LCSS_Quat(settings.lcss_distance, settings.lcss_epsilon,
                                                           settings.lcss_delta);
    algorithms_results[FRECHET] = analysis->perform_FRECHET_Pos(settings.frechet_distance);
    algorithms_results[FRECHET_QUAT] = analysis->perform_FRECHET_Quat(settings.frechet_distance);
    algorithms_results[WDDTW] = std::get<float>(wddtw_alignment);
}

void TrajectoryAnalysisManager::updateContext() {
    context->reference_file = cropString(refFile);
    context->input_file = cropString(inputFile);
    int n = inputTrajectories->get_anglesTrajectories().size();
    int m = refTrajectories->get_anglesTrajectories().size();
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
    // Weighted Derivative DTW
    auto wddtw_segments = calcSegmentsAligned(std::get<1>(wddtw_alignment), inputFrames, refFrames);
    auto wddtw_matching = new Matching(CDTW, std::get<2>(wddtw_alignment), std::get<1>(wddtw_alignment), wddtw_segments,
                                       n,
                                       m, std::get<0>(wddtw_alignment));
    context->matching_algorithms[WEIGHTDDTW] = wddtw_matching;
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