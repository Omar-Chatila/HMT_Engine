#ifndef DISTANCEMEASURES_HMT_LAYER_DATA_H
#define DISTANCEMEASURES_HMT_LAYER_DATA_H

#include <vector>
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
};

#endif //DISTANCEMEASURES_HMT_LAYER_DATA_H
