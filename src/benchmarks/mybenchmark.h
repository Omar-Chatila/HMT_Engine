// Updated mybenchmark.h
#ifndef DISTANCEMEASURES_HMT_MYBENCHMARK_H
#define DISTANCEMEASURES_HMT_MYBENCHMARK_H

#include <vector>
#include <filesystem>
#include <regex>
#include <string>
#include <algorithm>
#include <iostream>

#include "input_parser.h"
#include "trajectories.h"
#include "dtw.h"
#include "lcss.h"
#include "frechet.h"
#include "edit_distance.h"

constexpr const char* rootDirectory = "../../resources/motion_data/";
std::vector<std::string> input_files;
std::vector<std::string> ref_files;
std::vector<std::string> squat_files;

std::vector<std::vector<Quaternion*>> in_quat_frames;
std::vector<std::vector<Quaternion*>> ref_quat_frames;

enum Activity {
    SQUATS,
    TAI_CHI
};

void initFileLocations(enum Activity activity) {
    std::cout << "Initializing file locations" << std::endl;
    std::string activity_folder;
    if (activity == SQUATS) {
        activity_folder = "squats";
    } else if (activity == TAI_CHI) {
        activity_folder = "taichi";
    }

    std::string activity_path = std::string(rootDirectory) + activity_folder;
    std::regex input_file_regex(".*\\.txt");
    std::regex ref_file_regex("expertise.*\\.txt");

    for (const auto& entry : std::filesystem::directory_iterator(activity_path)) {
        std::string file_name = entry.path().filename().string();
        if (std::regex_match(file_name, ref_file_regex)) {
            ref_files.push_back(entry.path().string());
        } else if (std::regex_match(file_name, input_file_regex)) {
            input_files.push_back(entry.path().string());
        }
    }
    squat_files.reserve(input_files.size() + ref_files.size());
    squat_files.insert(squat_files.end(), input_files.begin(), input_files.end());
    squat_files.insert(squat_files.end(), ref_files.begin(), ref_files.end());
}

void initTrajectories() {
    std::cout << "Initializing Trajectories" << std::endl;
    // Init input trajectories
    for (auto file : input_files) {
        Input_parser *p = new Input_parser(file.c_str());
        std::vector<Frame> frames = p->get_frames();
        Trajectories *t = new Trajectories(frames);
        std::vector<Quaternion*> quats = t->get_anglesTrajectories();
        in_quat_frames.push_back(quats);
    }
    // Init ref trajectories
    for (auto file : ref_files) {
        Input_parser *p = new Input_parser(file.c_str());
        std::vector<Frame> frames = p->get_frames();
        Trajectories *t = new Trajectories(frames);
        std::vector<Quaternion*> quats = t->get_anglesTrajectories();
        ref_quat_frames.push_back(quats);
    }
}

#endif //DISTANCEMEASURES_HMT_MYBENCHMARK_H
