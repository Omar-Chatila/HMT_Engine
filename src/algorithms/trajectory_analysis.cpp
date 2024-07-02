#include "trajectory_analysis.h"

std::vector<std::function<float(const Vec3D &, const Vec3D &)>> vec_dist_measures = {
        euclid, squared_euclid, manhattan,
        [](const Vec3D &a, const Vec3D &b) { return minkowski(a, b, p); }, maximum_norm, minimum_distance,
        canberra_distance, scalar_product,
        cosine_similarity, cosine_distance1, cosine_distance2, hamming_distance};

std::function<float(const Quaternion *, const Quaternion *,
                    const std::array<bool, JOINT_COUNT> &selectedJ)> quaternion_dist = quat_dist;
std::function<float(const Vec3D *, const Vec3D *)> vector_dist = vec_dist;
std::vector<std::function<float(const Quaternion *, const Quaternion *)>> error_dist = {
        Joint::too_deep_dist,
        Joint::feet_too_close_dist,
        Joint::incorrect_weight_dist,
        Joint::hips_dont_start_dist,
        Joint::movement_dyn_dist,
        Joint::arched_neck_dist,
        Joint::hollow_back_dist,
        Joint::knees_sideways_dist,
        Joint::symmetry_dist
};

std::tuple<float, std::vector<int>, float *> Trajectoy_analysis::perform_DTW(Joint::Type joint, Distances type) {
    float *c_matrix = Dtw::dtw(input_trajectories.get_positionsTrajectory(joint),
                               reference_trajectories.get_positionsTrajectory(joint),
                               input_trajectories.size(), reference_trajectories.size(),
                               vec_dist_measures[type]);
    auto pair = Dtw::get_cost_and_alignment(c_matrix, input_trajectories.size(), reference_trajectories.size());
    return {pair.first, pair.second, c_matrix};
}

std::tuple<float, std::vector<int>, float *>
Trajectoy_analysis::perform_DTW(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                                Distances dist) {
    float *c_matrix = Dtw::dtw(inp_traj, ref_traj, quaternion_dist);
    auto pair = Dtw::get_cost_and_alignment(c_matrix, inp_traj.size(), ref_traj.size());
    return {pair.first, pair.second, c_matrix};
}

std::tuple<float, std::vector<int>, float *>
Trajectoy_analysis::perform_WDTW(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                                 float g, float w_max,
                                 Distances dist) {
    float *c_matrix = Dtw::wdtw(inp_traj, ref_traj, quaternion_dist, g, w_max);
    auto pair = Dtw::get_cost_and_alignment(c_matrix, inp_traj.size(), ref_traj.size());
    return {pair.first, pair.second, c_matrix};
}

std::tuple<float, std::vector<int>, float *>
Trajectoy_analysis::perform_WDDTW(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                                  float g,
                                  float w_max, Distances dist) {
    float *c_matrix = Dtw::wddtw(inp_traj, ref_traj, quaternion_dist, g, w_max);
    auto pair = Dtw::get_cost_and_alignment(c_matrix, inp_traj.size(), ref_traj.size());
    return {pair.first, pair.second, c_matrix};
}

// Perform EDR on specific joint
float Trajectoy_analysis::perform_EDR(Joint::Type joint, Distances type, float epsilon) {
    return EditDistance::edr(input_trajectories.get_positionsTrajectory(joint),
                             reference_trajectories.get_positionsTrajectory(joint),
                             input_trajectories.size(), reference_trajectories.size(), epsilon,
                             vec_dist_measures[type]);
}

//Perform EDR on all Joint positions
float Trajectoy_analysis::perform_EDR_Pos(Distances type, float epsilon) {
    return EditDistance::edr(input_trajectories.get_positions_trajectories(),
                             reference_trajectories.get_positions_trajectories(),
                             epsilon, vector_dist);
}

//Perform EDR on all Joint Angles
float Trajectoy_analysis::perform_EDR_Quat(Distances type, float epsilon) {
    return EditDistance::edr(input_trajectories.get_anglesTrajectories(),
                             reference_trajectories.get_anglesTrajectories(),
                             epsilon, quaternion_dist);
}

//Perform TWED on all Joint angles
float Trajectoy_analysis::perform_TWED_Quat(Distances type, float nu, float lambda) {
    return EditDistance::twed(input_trajectories.get_anglesTrajectories(),
                              reference_trajectories.get_anglesTrajectories(),
                              nu, lambda, quaternion_dist);
}

//Perform LCSS on all Joint angles
float Trajectoy_analysis::perform_LCSS_Quat(Distances type, float epsilon, float delta) {
    return LCSS::lcss(input_trajectories.get_anglesTrajectories(),
                      reference_trajectories.get_anglesTrajectories(),
                      epsilon, delta, quaternion_dist);
}

// Perform Frechet distance on all Joint angles
float Trajectoy_analysis::perform_FRECHET_Quat(Distances type) {
    return Frechet::frechet(input_trajectories.get_anglesTrajectories(),
                            reference_trajectories.get_anglesTrajectories(), quaternion_dist);
}

float Trajectoy_analysis::perform_FRECHET_Pos(Distances type) {
    return Frechet::frechet(input_trajectories.get_positions_trajectories(),
                            reference_trajectories.get_positions_trajectories(), vector_dist);
}


