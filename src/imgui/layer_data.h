#ifndef DISTANCEMEASURES_HMT_LAYER_DATA_H
#define DISTANCEMEASURES_HMT_LAYER_DATA_H

#include <vector>
#include <array>
#include <string>
#include "trajectory_analysis_manager.h"
#include "motion_file_processor.h"

struct TrajectoryInfo {
    std::string reference;
    std::vector<float> costs;
    TrajectoryAnalysisManager *manager;
};

class SharedData {
public:
    MotionFileProcessor *currentAnalysis;
    std::vector<TrajectoryInfo> trajectoryInfos;
    std::array<int, 5> inp_segments;
    std::array<int, 5> ref_segments;
    std::array<int, 10> alignedSegments;
    std::array<int, 10> wdtw_alignedSegments;
    std::array<int, 10> wddtw_alignedSegments;
};

#endif //DISTANCEMEASURES_HMT_LAYER_DATA_H
