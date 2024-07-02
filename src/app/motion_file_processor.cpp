#include "motion_file_processor.h"

MotionFileProcessor::MotionFileProcessor(Activity p_activity) : activity(p_activity) {
    initFileLocations();
}


MotionFileProcessor::~MotionFileProcessor() {
    std::for_each(trajectoryManagers.begin(), trajectoryManagers.end(), [](TrajectoryAnalysisManager *manager) {
        delete manager;
    });
    trajectoryManagers.clear();
}

void MotionFileProcessor::initFileLocations() {
    std::string activity_folder;
    if (activity == SQUATS) {
        activity_folder = "squats";
    } else if (activity == TAI_CHI) {
        activity_folder = "taichi";
    }

    std::string activity_path = std::string(rootDirectory) + activity_folder;
    std::regex input_file_regex(".*\\.txt");
    std::regex ref_file_regex("expertise.*\\.txt");

    for (const auto &entry: std::filesystem::directory_iterator(activity_path)) {
        std::string file_name = entry.path().filename().string();
        if (std::regex_match(file_name, ref_file_regex)) {
            this->ref_files.push_back(entry.path().string());
        } else if (std::regex_match(file_name, input_file_regex)) {
            this->input_files.push_back(entry.path().string());
        }
    }
}

void MotionFileProcessor::processAllFiles() {
    for (auto &manager: trajectoryManagers) delete manager;
    trajectoryManagers.clear();
    std::cout << "Create all Trajectories" << std::endl;
    for (const auto &inputFile: input_files) {
        auto *c = new UIContext();
        for (const auto &refFile: ref_files) {
            auto *manager = new TrajectoryAnalysisManager(inputFile, refFile, c);
            manager->performAnalysis();
            trajectoryManagers.push_back(manager);
        }
    }
}

void MotionFileProcessor::processInputFile(const std::string &input) {
    for (auto &manager: trajectoryManagers) delete manager;
    trajectoryManagers.clear();
    std::string inp_file;
    if (activity == SQUATS) {
        inp_file = std::string(rootDirectory) + "squats/" + input;
    } else {
        inp_file = std::string(rootDirectory) + "taichi/" + input;
    }
    for (const auto &refFile: ref_files) {
        auto *context = new UIContext();
        auto *manager = new TrajectoryAnalysisManager(inp_file, refFile, context);
        manager->performAnalysis();
        trajectoryManagers.push_back(manager);
    }
}

void MotionFileProcessor::updateTrajectoryManagers() {
    for (auto &manager: trajectoryManagers) {
        manager->performAnalysis();
    }
}

std::vector<TrajectoryAnalysisManager *> MotionFileProcessor::getKClosestMatches(int k, enum Algorithm algorithm) {
    std::vector<std::pair<float, TrajectoryAnalysisManager *>> costs;
    for (const auto manager: trajectoryManagers) {
        float cost = manager->getAlgorithmResult(algorithm);
        costs.emplace_back(cost, manager);
    }
    std::sort(costs.begin(), costs.end(), [](const auto &a, const auto &b) {
        return a.first < b.first;
    });

    std::vector<TrajectoryAnalysisManager *> kClosest;
    for (int i = 0; i < k && i < costs.size(); ++i) {
        kClosest.push_back(costs[i].second);
    }

    for (auto manager: kClosest) {
        manager->updateContext();
    }

    return kClosest;
}
