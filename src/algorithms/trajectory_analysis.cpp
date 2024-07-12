#include "trajectory_analysis.h"

std::tuple<float, std::vector<int>, float *>
Trajectoy_analysis::perform_DTW(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                                Distances dist) {
    float *c_matrix;
    if (dist == QUATERNION)
        c_matrix = Dtw::dtw(inp_traj, ref_traj, quaternion_dist);
    else if (dist == QUATERNION_WEIGHTED)
        c_matrix = Dtw::dtw(inp_traj, ref_traj, quaternion_dist_weighted);

    auto pair = Dtw::get_cost_and_alignment(c_matrix, inp_traj.size(), ref_traj.size());
    return {pair.first, pair.second, c_matrix};
}

std::tuple<float, std::vector<int>, float *>
Trajectoy_analysis::perform_standardized_DTW(const vector<Quaternion *> &inp_traj, const vector<Quaternion *> &ref_traj,
                                             Distances dist) {
    auto inp_max = Trajectories::max_rotations(inp_traj);
    auto ref_max = Trajectories::max_rotations(ref_traj);
    std::vector<Quaternion *> normalized_inp_traj = Dtw::normalize_trajectory(inp_traj, inp_max);
    std::vector<Quaternion *> normalized_ref_traj = Dtw::normalize_trajectory(ref_traj, ref_max);
    float *c_matrix;
    if (dist == QUATERNION)
        c_matrix = Dtw::standardized_dtw(normalized_inp_traj, normalized_ref_traj, quaternion_dist);
    else if (dist == QUATERNION_WEIGHTED)
        c_matrix = Dtw::standardized_dtw(normalized_inp_traj, normalized_ref_traj, quaternion_dist_weighted);
    auto pair = Dtw::get_cost_and_alignment(c_matrix, inp_traj.size(), ref_traj.size());
    std::cout << pair.second.size() << std::endl;
    return {pair.first, pair.second, c_matrix};
}

std::tuple<float, std::vector<int>, float *>
Trajectoy_analysis::perform_WDTW(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                                 float g, float w_max,
                                 Distances dist) {
    float *c_matrix;
    if (dist == QUATERNION)
        c_matrix = Dtw::wdtw(inp_traj, ref_traj, quaternion_dist, g, w_max);
    else if (dist == QUATERNION_WEIGHTED)
        c_matrix = Dtw::wdtw(inp_traj, ref_traj, quaternion_dist_weighted, g, w_max);
    auto pair = Dtw::get_cost_and_alignment(c_matrix, inp_traj.size(), ref_traj.size());
    return {pair.first, pair.second, c_matrix};
}

std::tuple<float, std::vector<int>, float *>
Trajectoy_analysis::perform_WDDTW(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                                  float g,
                                  float w_max, Distances dist) {
    float *c_matrix;
    if (dist == QUATERNION)
        c_matrix = Dtw::wddtw(inp_traj, ref_traj, quaternion_dist, g, w_max);
    else if (dist == QUATERNION_WEIGHTED)
        c_matrix = Dtw::wddtw(inp_traj, ref_traj, quaternion_dist_weighted, g, w_max);
    auto pair = Dtw::get_cost_and_alignment(c_matrix, inp_traj.size(), ref_traj.size());
    return {pair.first, pair.second, c_matrix};
}

//Perform EDR on all Joint positions
float Trajectoy_analysis::perform_EDR_Pos(Distances type, float epsilon) {
    return EditDistance::edr(input_trajectories.get_positions_trajectories(),
                             reference_trajectories.get_positions_trajectories(),
                             epsilon, vector_dist);
}

//Perform EDR on all Joint Angles
float Trajectoy_analysis::perform_EDR_Quat(Distances type, float epsilon) {
    if (type == QUATERNION)
        return EditDistance::edr(input_trajectories.get_anglesTrajectories(),
                                 reference_trajectories.get_anglesTrajectories(),
                                 epsilon, quaternion_dist);
    else if (type == QUATERNION_WEIGHTED)
        return EditDistance::edr(input_trajectories.get_anglesTrajectories(),
                                 reference_trajectories.get_anglesTrajectories(),
                                 epsilon, quaternion_dist_weighted);
    return 0.0f;
}

//Perform TWED on all Joint angles
float Trajectoy_analysis::perform_TWED_Quat(Distances type, float nu, float lambda) {
    if (type == QUATERNION)
        return EditDistance::twed(input_trajectories.get_anglesTrajectories(),
                                  reference_trajectories.get_anglesTrajectories(),
                                  nu, lambda, quaternion_dist);
    else if (type == QUATERNION_WEIGHTED)
        return EditDistance::twed(input_trajectories.get_anglesTrajectories(),
                                  reference_trajectories.get_anglesTrajectories(),
                                  nu, lambda, quaternion_dist_weighted);
    return 0.0f;
}

//Perform LCSS on all Joint angles
float Trajectoy_analysis::perform_LCSS_Quat(Distances type, float epsilon, float delta) {
    if (type == QUATERNION)
        return LCSS::lcss(input_trajectories.get_anglesTrajectories(),
                          reference_trajectories.get_anglesTrajectories(),
                          epsilon, delta, quaternion_dist);
    else if (type == QUATERNION_WEIGHTED)
        return LCSS::lcss(input_trajectories.get_anglesTrajectories(),
                          reference_trajectories.get_anglesTrajectories(),
                          epsilon, delta, quaternion_dist_weighted);
    return 0.0f;
}

// Perform Frechet distance on all Joint angles
float Trajectoy_analysis::perform_FRECHET_Quat(Distances type) {
    if (type == QUATERNION)
        return Frechet::frechet(input_trajectories.get_anglesTrajectories(),
                                reference_trajectories.get_anglesTrajectories(), quaternion_dist);
    else if (type == QUATERNION_WEIGHTED)
        return Frechet::frechet(input_trajectories.get_anglesTrajectories(),
                                reference_trajectories.get_anglesTrajectories(), quaternion_dist_weighted);
    return 0.0f;
}

float Trajectoy_analysis::perform_FRECHET_Pos(Distances type) {
    return Frechet::frechet(input_trajectories.get_positions_trajectories(),
                            reference_trajectories.get_positions_trajectories(), vector_dist);
}




