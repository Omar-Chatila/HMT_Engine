#ifndef TRAJECTOY_ANALYSIS_H
#define TRAJECTOY_ANALYSIS_H

#include <functional>
#include "trajectories.h"
#include "util.h"
#include <array>
#include "distance_functions.h"
#include "distance_measures.h"
#include "dtw.h"
#include <vector>
#include "edit_distance.h"
#include "lcss.h"
#include "frechet.h"
#include "error_distances.h"

class Trajectoy_analysis {
private:
    Trajectories input_trajectories;
    Trajectories reference_trajectories;
public:
    Trajectoy_analysis(const Trajectories &p_input, const Trajectories &p_reference)
            : input_trajectories{p_input}, reference_trajectories{p_reference} {}

    ~Trajectoy_analysis() = default;

    std::tuple<float, std::vector<int>, float *>
    perform_DTW(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj, Distances dist);

    std::tuple<float, std::vector<int>, float *>
    perform_SSC1_DTW(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                     Distances dist);

    std::tuple<float, std::vector<int>, float *>
    perform_SSC2_DTW(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                     Distances dist);

    std::tuple<float, std::vector<int>, float *>
    perform_LW_DTW(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                   Distances dist, Vec3D &localWeights);

    std::tuple<float, std::vector<int>, float *>
    perform_standardized_DTW(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                             Distances dist);

    std::tuple<float, std::vector<int>, float *>
    perform_WDTW(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj, float g,
                 float w_max,
                 Distances dist);

    std::tuple<float, std::vector<int>, float *>
    perform_WDDTW(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj, float g,
                  float w_max, Distances dist);

    float perform_EDR_Pos(Distances type, float epsilon);

    float perform_EDR_Quat(Distances type, float epsilon);

    float perform_TWED_Quat(Distances type, float nu, float lambda);

    float perform_LCSS_Quat(Distances type, float epsilon, float delta);

    float perform_FRECHET_Quat(Distances type);

    float perform_FRECHET_Pos(Distances type);

};

#endif