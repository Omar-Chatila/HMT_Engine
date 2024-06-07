#ifndef DISTANCEMEASURES_HMT_MOTION_FILE_PROCESSOR_H
#define DISTANCEMEASURES_HMT_MOTION_FILE_PROCESSOR_H

#include <vector>
#include <filesystem>
#include <regex>
#include <string>
#include <algorithm>
#include "trajectory_analysis_manager.h"

enum Activity {
    SQUATS,
    TAI_CHI
};

class MotionFileProcessor {
private:
    static constexpr const char* rootDirectory = "resources/motion_data/";
    Activity activity;
    std::vector<TrajectoryAnalysisManager *> trajectoryManagers;
    std::vector<std::string> input_files;
    std::vector<std::string> ref_files;

    void initFileLocations();

public:
    explicit MotionFileProcessor(enum Activity p_activity);
    ~MotionFileProcessor();
    void processAllFiles();
    void processInputFile(const std::string &input);
    TrajectoryAnalysisManager* getClosestMatch(enum Algorithm algorithm);
};


#endif //DISTANCEMEASURES_HMT_MOTION_FILE_PROCESSOR_H
