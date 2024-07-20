#pragma once

#include "trajectory_analysis.h"
#include "input_parser.h"
#include "algo_settings.h"
#include "util.h"
#include <vector>
#include <map>
#include <array>
#include <string>
#include <tuple>
#include <iostream>

static constexpr int ALGO_COUNT = 12;

enum Algorithm {
    DTW,
    //STANDARDIZED_DTW,
    WDTW,
    WDDTW,
    SSC1_DTW,
    SSC2_DTW,
    LW_DTW,
    EDR,
    TWED,
    FRECHET,
    FRECHET_QUAT,
    LC_FRECHET,
    LCSS
};

constexpr int MATCHING_COUNT = 6;
enum MatchingType {
    CDTW,
    WEIGHTDTW,
    WEIGHTDDTW,
    SS_CONDITION1DTW,
    SS_CONDITION2DTW,
    LOCAL_WEIGHTS_DTW,
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
                                      alignmentCost(p_alignmentCost) {
    }

    ~Matching() {
        free(distMatrix);
    }
};

class TrajectoryAnalysisManager {
public:
    TrajectoryAnalysisManager(std::string inputFile, std::string refFile,
                              Trajectories *p_inputTrajectories, Trajectories *p_refTrajectories);

    ~TrajectoryAnalysisManager();

    void performAnalysis();

    void setSegmentsAndMatchings();

    float getAlgorithmResult(enum Algorithm algorithm);

    std::vector<Frame> getInputFrames();

    std::vector<Frame> getRefFrames();

    std::array<Matching *, MATCHING_COUNT> matching_algorithms;
    std::array<int, 5> inpSegments;
    std::array<int, 5> refSegments;
    std::string inputFile;
    std::string refFile;
    std::array<float, ALGO_COUNT> algorithms_results;
    Trajectories *inputTrajectories;
    Trajectories *refTrajectories;
private:
    std::vector<Frame> inputFrames;
    std::vector<Frame> refFrames;
    std::tuple<float, std::vector<int>, float *> alignment;
    std::tuple<float, std::vector<int>, float *> ssc1_alignment;
    std::tuple<float, std::vector<int>, float *> ssc2_alignment;
    std::tuple<float, std::vector<int>, float *> lw_alignment;
    std::tuple<float, std::vector<int>, float *> wdtw_alignment;
    std::tuple<float, std::vector<int>, float *> wddtw_alignment;
    std::tuple<float, std::vector<int>, float *> standard_DTW_alignment;
    Trajectoy_analysis *analysis;
};
