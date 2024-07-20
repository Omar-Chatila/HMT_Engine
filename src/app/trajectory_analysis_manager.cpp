#include "trajectory_analysis_manager.h"
#include <imgui.h>
#include <utility>

TrajectoryAnalysisManager::TrajectoryAnalysisManager(std::string inputFile, std::string refFile,
                                                     Trajectories *p_inputTrajectories, Trajectories *p_refTrajectories)
        : inputFile(std::move(inputFile)), refFile(std::move(refFile)), inputTrajectories(p_inputTrajectories),
          refTrajectories(p_refTrajectories), analysis(nullptr) {
    this->refFrames = this->refTrajectories->getFrames();
    this->inputFrames = this->inputTrajectories->getFrames();
    analysis = new Trajectoy_analysis(*p_inputTrajectories, *p_refTrajectories);
    srand(0);
}

TrajectoryAnalysisManager::~TrajectoryAnalysisManager() {
    delete refTrajectories;
    delete inputTrajectories;
    delete analysis;
}

void TrajectoryAnalysisManager::performAnalysis() {
    AlgoSettings &settings = AlgoSettings::getInstance();
    alignment = analysis->perform_DTW(
            inputTrajectories->get_anglesTrajectories(),
            refTrajectories->get_anglesTrajectories(), settings.dtw_distance);
    IM_ASSERT(!std::get<1>(alignment).empty());
    wdtw_alignment = analysis->perform_WDTW(inputTrajectories->get_anglesTrajectories(),
                                            refTrajectories->get_anglesTrajectories(), settings.wdtw_g,
                                            settings.wdtw_w_max, settings.wdtw_distance);
    wddtw_alignment = analysis->perform_WDDTW(inputTrajectories->get_anglesTrajectories(),
                                              refTrajectories->get_anglesTrajectories(), settings.wddtw_g,
                                              settings.wddtw_w_max, settings.wddtw_distance);
    /*standard_DTW_alignment = analysis->perform_standardized_DTW(inputTrajectories->get_anglesTrajectories(),
                                                                refTrajectories->get_anglesTrajectories(),
                                                                settings.dtw_distance); */
    ssc1_alignment = analysis->perform_SSC1_DTW(inputTrajectories->get_anglesTrajectories(),
                                                refTrajectories->get_anglesTrajectories(), settings.dtw_distance);
    ssc2_alignment = analysis->perform_SSC2_DTW(inputTrajectories->get_anglesTrajectories(),
                                                refTrajectories->get_anglesTrajectories(), settings.dtw_distance);
    lw_alignment = analysis->perform_LW_DTW(inputTrajectories->get_anglesTrajectories(),
                                            refTrajectories->get_anglesTrajectories(), settings.dtw_distance,
                                            settings.localWeights);
    algorithms_results[DTW] = std::get<0>(alignment);
    algorithms_results[WDTW] = std::get<float>(wdtw_alignment);
    algorithms_results[WDDTW] = std::get<float>(wddtw_alignment);
    algorithms_results[SSC1_DTW] = std::get<float>(ssc1_alignment);
    algorithms_results[SSC2_DTW] = std::get<float>(ssc2_alignment);
    //algorithms_results[STANDARDIZED_DTW] = std::get<float>(standard_DTW_alignment);
    algorithms_results[LW_DTW] = std::get<float>(lw_alignment);
    algorithms_results[EDR] = analysis->perform_EDR_Quat(settings.edr_distance,
                                                         settings.edr_epsilon);
    algorithms_results[TWED] = analysis->perform_TWED_Quat(settings.twed_distance,
                                                           settings.twed_nu,
                                                           settings.twed_lambda);
    algorithms_results[LCSS] = analysis->perform_LCSS_Quat(settings.lcss_distance,
                                                           settings.lcss_epsilon,
                                                           settings.lcss_delta);
    algorithms_results[FRECHET] = analysis->perform_FRECHET_Pos(
            settings.frechet_distance);
    algorithms_results[FRECHET_QUAT] = analysis->perform_FRECHET_Quat(
            settings.frechet_distance);
    // TODO: Placeholder for LCDFM
    algorithms_results[LC_FRECHET] = static_cast<float>(rand() % 10);
}

void TrajectoryAnalysisManager::setSegmentsAndMatchings() {
    int n = static_cast<int>(inputTrajectories->get_anglesTrajectories().size());
    int m = static_cast<int>(refTrajectories->get_anglesTrajectories().size());
    // Classic DTW
    auto cdtw_segments = calcSegmentsAligned(std::get<1>(alignment), inputFrames, refFrames);
    auto cdtw_matching = new Matching(CDTW, std::get<2>(alignment), std::get<1>(alignment), cdtw_segments, n, m,
                                      std::get<0>(alignment));
    this->matching_algorithms[CDTW] = cdtw_matching;

    // LW-DTW
    auto lwdtw_segments = calcSegmentsAligned(std::get<1>(lw_alignment), inputFrames, refFrames);
    auto lwdtw_matching = new Matching(LOCAL_WEIGHTS_DTW, std::get<2>(lw_alignment), std::get<1>(lw_alignment),
                                       lwdtw_segments, n, m,
                                       std::get<0>(lw_alignment));
    this->matching_algorithms[LW_DTW] = lwdtw_matching;
    this->matching_algorithms[SSC2_DTW] = lwdtw_matching;
    this->matching_algorithms[SSC1_DTW] = lwdtw_matching;
    // SSC1 DTW
    /*auto ssc1_dtw_segments = calcSegmentsAligned(std::get<1>(ssc1_alignment), inputFrames, refFrames);
    auto ssc1_dtw_matching = new Matching(SS_CONDITION1DTW, std::get<2>(ssc1_alignment), std::get<1>(ssc1_alignment),
                                          ssc1_dtw_segments, n, m,
                                          std::get<0>(ssc1_alignment));
    this->matching_algorithms[SS_CONDITION1DTW] = ssc1_dtw_matching;


    auto stand_dtw_segments = calcSegmentsAligned(std::get<1>(standard_DTW_alignment), inputFrames, refFrames);
    auto stand_dtw_matching = new Matching(STAND_DTW, std::get<2>(standard_DTW_alignment),
                                           std::get<1>(standard_DTW_alignment), stand_dtw_segments, n, m,
                                           std::get<0>(ssc1_alignment));
    this->matching_algorithms[STAND_DTW] = stand_dtw_matching; */
    // Weighted DTW
    auto wdtw_segments = calcSegmentsAligned(std::get<1>(wdtw_alignment), inputFrames, refFrames);
    auto wdtw_matching = new Matching(WEIGHTDTW, std::get<2>(wdtw_alignment), std::get<1>(wdtw_alignment),
                                      wdtw_segments, n,
                                      m, std::get<0>(wdtw_alignment));
    this->matching_algorithms[WEIGHTDTW] = wdtw_matching;
    // Weighted Derivative DTW
    auto wddtw_segments = calcSegmentsAligned(std::get<1>(wddtw_alignment), inputFrames, refFrames);
    auto wddtw_matching = new Matching(WEIGHTDDTW, std::get<2>(wddtw_alignment), std::get<1>(wddtw_alignment),
                                       wddtw_segments,
                                       n,
                                       m, std::get<0>(wddtw_alignment));
    this->matching_algorithms[WEIGHTDDTW] = wddtw_matching;
    this->inpSegments = calculateSegments(inputFrames);
    this->refSegments = calculateSegments(refFrames);
}

float TrajectoryAnalysisManager::getAlgorithmResult(enum Algorithm algorithm) {
    return algorithms_results[algorithm];
}

std::vector<Frame> TrajectoryAnalysisManager::getInputFrames() {
    return this->inputFrames;
}

std::vector<Frame> TrajectoryAnalysisManager::getRefFrames() {
    return this->refFrames;
}


