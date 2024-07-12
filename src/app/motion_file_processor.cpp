#include "motion_file_processor.h"

MotionFileProcessor::MotionFileProcessor(Activity p_activity) : activity(p_activity) {
    initFileLocations();
}

MotionFileProcessor::~MotionFileProcessor() {
    for (auto &entry: trajectoryManagers)
        delete entry.second;
}

void MotionFileProcessor::initFileLocations() {
    std::string activity_folder;
    if (activity == SQUATS) {
        activity_folder = "squats";
    } else if (activity == TAI_CHI) {
        activity_folder = "taichi";
    }

    std::string activity_path = std::filesystem::path(rootDirectory + activity_folder).generic_string();
    std::regex input_file_regex(".*\\.txt");
    std::regex ref_file_regex("expertise.*\\.txt");

    for (const auto &entry: std::filesystem::directory_iterator(activity_path)) {
        std::string file_name = entry.path().filename().generic_string(); // Ensure consistent filename format
        std::string full_path = entry.path().generic_string(); // Ensure consistent path format

        if (std::regex_match(file_name, ref_file_regex)) {
            this->ref_files.push_back(full_path);
        } else if (std::regex_match(file_name, input_file_regex)) {
            this->input_files.push_back(full_path);
        }
    }
}

void MotionFileProcessor::processAllFiles() {
    size_t totalFiles = input_files.size() + ref_files.size();
    size_t processedFiles = 0;

    // Process input files
    for (const auto &inputFile: input_files) {
        std::cout << "Processing input file " << ++processedFiles << " of " << totalFiles << ": " << inputFile
                  << std::endl;
        auto *inputParser = new Input_parser(inputFile.c_str());
        std::vector<Frame> inputFrames = inputParser->get_frames();
        auto *current_inp_trajectories = new Trajectories(inputFrames);
        delete inputParser;
        input_trajectories[inputFile].insert(current_inp_trajectories);
    }

    // Process reference files
    for (const auto &refFile: ref_files) {
        std::cout << "Processing reference file " << ++processedFiles << " of " << totalFiles << ": " << refFile
                  << std::endl;
        auto *refParser = new Input_parser(refFile.c_str());
        std::vector<Frame> refFrames = refParser->get_frames();
        auto *current_ref_trajectories = new Trajectories(refFrames);
        delete refParser;
        reference_trajectories[refFile].insert(current_ref_trajectories);
    }
}

void MotionFileProcessor::createTrajectoryAnalysisManagers() {
    for (const auto &inputFile: input_files) {
        for (const auto &refFile: ref_files) {
            Trajectories *inputTrajectories = *input_trajectories[inputFile].begin();
            Trajectories *refTrajectories = *reference_trajectories[refFile].begin();
            trajectoryManagers[{inputFile, refFile}] = new TrajectoryAnalysisManager(inputFile, refFile,
                                                                                     inputTrajectories,
                                                                                     refTrajectories);
        }
    }
}

void MotionFileProcessor::processInputFile(const std::string &input) {
    std::string inputFile;
    trajectoryInfos.clear();
    inputFile = adjustString(input);
    for (const auto &refFile: ref_files) {
        TrajectoryInfo info;
        info.reference = refFile;
        auto key = std::make_pair(inputFile, refFile);
        auto it = trajectoryManagers.find(key);
        IM_ASSERT(it != trajectoryManagers.end());
        TrajectoryAnalysisManager *manager = it->second;
        info.manager = manager;
        manager->performAnalysis();
        for (int i = 0; i < ALGO_COUNT; i++) {
            float cost = manager->getAlgorithmResult(static_cast<Algorithm>(i));
            info.costs.push_back(cost);
        }
        trajectoryInfos.push_back(info);
    }
}

std::vector<TrajectoryAnalysisManager *>
MotionFileProcessor::getKClosestMatches(int k, const std::string &input, Algorithm algorithm) {
    std::string inputFile = adjustString(input);
    std::vector<TrajectoryAnalysisManager *> kClosest;
    for (const auto &[filePair, second]: trajectoryManagers) {
        if (filePair.first == inputFile)
            kClosest.emplace_back(second);
    }
    IM_ASSERT(!kClosest.empty());
    std::sort(kClosest.begin(), kClosest.end(),
              [&algorithm](const TrajectoryAnalysisManager *a, const TrajectoryAnalysisManager *b) {
                  return a->algorithms_results[static_cast<int>(algorithm)] <
                         b->algorithms_results[static_cast<int>(algorithm)];
              });
    return kClosest;
}
