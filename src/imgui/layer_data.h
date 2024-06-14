#ifndef DISTANCEMEASURES_HMT_LAYER_DATA_H
#define DISTANCEMEASURES_HMT_LAYER_DATA_H

#include <vector>
#include <array>
#include <string>
#include "trajectory_analysis_manager.h"

struct TrajectoryInfo {
    std::string reference;
    std::vector<float> costs;
    TrajectoryAnalysisManager* manager;
};

class SharedData {
public:
    std::vector<TrajectoryInfo> trajectoryInfos;
    std::array<int, 5> inp_segments;
    std::array<int, 5> ref_segments;
    std::array<int, 10> alignedSegments;
};

#endif //DISTANCEMEASURES_HMT_LAYER_DATA_H
