#ifndef TRAJECTOY_ANALYSIS_H
#define TRAJECTOY_ANALYSIS_H

#include <functional>
#include "trajectories.h"
#include "util.h"
#include "distance_measures.h"
#include "dtw.h"
#include <vector>
#include "edit_distance.h"
#include "error_distances.h"

// For Minkonwski
const float p = 3;

extern std::vector<std::function<float(const Vec3D&, const Vec3D&)>> vec_dist_measures;
extern std::function<float(const Quaternion*, const Quaternion*)> quaternion_dist;
extern std::function<float(const Vec3D*, const Vec3D*)> vector_dist;
extern std::vector<std::function<float(const Quaternion*, const Quaternion*)>> error_dist;

class Trajectoy_analysis {
private:
    Trajectories input_trajectories;
    Trajectories reference_trajectories;
public:
    Trajectoy_analysis(const Trajectories &p_input, const Trajectories &p_reference) 
        : input_trajectories{p_input}, reference_trajectories{p_reference} {}
    ~Trajectoy_analysis() {};

    std::tuple<float, std::vector<int>, float*> perform_DTW(Joint::Type joint, Distances type);
    std::tuple<float, std::vector<int>, float*> perform_DTW(const std::vector<Quaternion*> &inp_traj, const std::vector<Quaternion*> &ref_traj);
    float perform_EDR(Joint::Type joint, Distances type, float epsilon);
    float perform_EDR_Pos(Distances type, float epsilon);
    float perform_EDR_Quat(Distances type, float epsilon);
    float perform_ErrorDetection(const std::vector<Quaternion*> &inp_traj, const std::vector<Quaternion*> &ref_traj, ErrorPattern errorPattern);

};

#endif