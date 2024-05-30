#include "trajectory_analysis.h"

std::vector<std::function<double(const Vec3D&, const Vec3D&)>> dist_measures = {
        euclid, squared_euclid, manhattan,
        [](const Vec3D &a, const Vec3D &b) { return minkowski(a, b, p); }, maximum_norm, minimum_distance, 
        canberra_distance, scalar_product, 
        cosine_similarity, cosine_distance1, cosine_distance2, hamming_distance};

std::pair<double, std::vector<int>> Trajectoy_analysis::perform_DTW(Joint::Type joint, Distances type) {
    double* c_matrix = Dtw::dtw(input_trajectories.get_positionsTrajectory(joint), 
                                reference_trajectories.get_positionsTrajectory(joint), 
                                input_trajectories.size(), reference_trajectories.size(), 
                                dist_measures[type]);
    return Dtw::get_cost_and_alignment(c_matrix, input_trajectories.size(), reference_trajectories.size());
}

double Trajectoy_analysis::perform_EDR(Joint::Type joint, Distances type, double epsilon) {
    return EditDistance::edr(input_trajectories.get_positionsTrajectory(joint), 
                      reference_trajectories.get_positionsTrajectory(joint), 
                      input_trajectories.size(), reference_trajectories.size(), epsilon, dist_measures[type]);
}
