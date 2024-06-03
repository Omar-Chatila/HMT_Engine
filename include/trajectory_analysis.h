#ifndef TRAJECTOY_ANALYSIS_H
#define TRAJECTOY_ANALYSIS_H

#include <functional>
#include "trajectories.h"
#include "distance_measures.h"
#include "dtw.h"
#include "edit_distance.h"

// For Minkonwski
const float p = 3;

extern std::vector<std::function<float(const Vec3D&, const Vec3D&)>> vec_dist_measures;
extern std::function<float(const Quaternion*, const Quaternion*)> quaternion_dist;

class Trajectoy_analysis {
private:
    Trajectories input_trajectories;
    Trajectories reference_trajectories;
public:
    Trajectoy_analysis(const Trajectories &p_input, const Trajectories &p_reference) 
        : input_trajectories{p_input}, reference_trajectories{p_reference} {}
    ~Trajectoy_analysis() {};

    std::pair<float, std::vector<int>> perform_DTW(Joint::Type joint, Distances type);
    std::pair<float, std::vector<int>> perform_DTW(const std::vector<Quaternion*> &inp_traj, const std::vector<Quaternion*> &ref_traj);
    float perform_EDR(Joint::Type joint, Distances type, float epsilon);

};

#endif