#ifndef DISTANCEMEASURES_HMT_MOTION_FILE_PROCESSOR_H
#define DISTANCEMEASURES_HMT_MOTION_FILE_PROCESSOR_H

#include <vector>
#include <filesystem>
#include <regex>
#include <string>
#include <algorithm>
#include "motion_file_processor.h"
#include "trajectory_analysis_manager.h"
//#include "Engine.h" // this line fucks everything up!!!

enum Activity {
    SQUATS,
    TAI_CHI
};

class MotionFileProcessor {
private:
    static constexpr const char *rootDirectory = "../resources/motion_data/";
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

    void updateTrajectoryManagers();

    std::vector<TrajectoryAnalysisManager *> getKClosestMatches(int k, enum Algorithm algorithm);
};

#endif