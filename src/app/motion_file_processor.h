#ifndef DISTANCEMEASURES_HMT_MOTION_FILE_PROCESSOR_H
#define DISTANCEMEASURES_HMT_MOTION_FILE_PROCESSOR_H

#include <utility>
#include <vector>
#include <filesystem>
#include <regex>
#include <string>
#include <algorithm>
#include <map>
#include <set>
#include <thread>
#include <mutex>
#include "trajectory_analysis_manager.h"

enum Activity {
    SQUATS,
    TAI_CHI
};

struct TrajectoryInfo {
    std::string reference;
    std::vector<float> costs;
    TrajectoryAnalysisManager *manager;

    TrajectoryInfo() : manager(nullptr) {};

    TrajectoryInfo(std::string refFile, const std::vector<float> &costs, TrajectoryAnalysisManager *manager)
            : reference(std::move(refFile)), costs(costs), manager(manager) {}
};

class MotionFileProcessor {
private:
    static constexpr const char *rootDirectory = "../resources/motion_data/";
    Activity activity;
    std::vector<std::string> input_files;
    std::vector<std::string> ref_files;
    std::map<std::string, std::set<Trajectories *>> input_trajectories;
    std::map<std::string, std::set<Trajectories *>> reference_trajectories;
    std::map<std::pair<std::string, std::string>, TrajectoryAnalysisManager *> trajectoryManagers;

    void initFileLocations();

public:
    explicit MotionFileProcessor(enum Activity p_activity);

    ~MotionFileProcessor();

    void createTrajectoryAnalysisManagers();

    void processAllFiles();

    void processInputFile(const std::string &input);

    void updateTrajectoryManagers();

    std::vector<TrajectoryAnalysisManager *>
    getKClosestMatches(int k, const std::string &input, enum Algorithm algorithm);

    std::vector<TrajectoryInfo> trajectoryInfos;

private:
    std::string adjustString(const std::string &input) const {
        std::string result = input;
        if (!input.contains("resources")) {
            if (activity == SQUATS) {
                result = "../resources/motion_data/squats/" + input;
            } else {
                result = "../resources/motion_data/taichi/" + input;
            }
        }
        return result;
    }
};

#endif