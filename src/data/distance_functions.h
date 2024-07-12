#ifndef DISTANCEMEASURESHMT_DISTANCE_FUNCTIONS_H
#define DISTANCEMEASURESHMT_DISTANCE_FUNCTIONS_H

#include "distance_measures.h"
#include "error_distances.h"

// For Minkonwski
const float p = 3;
inline std::vector<std::function<float(const Vec3D &, const Vec3D &)>> vec_dist_measures = {
        euclid, squared_euclid, manhattan,
        [](const Vec3D &a, const Vec3D &b) { return minkowski(a, b, p); }, maximum_norm, minimum_distance,
        canberra_distance, scalar_product,
        cosine_similarity, cosine_distance1, cosine_distance2, hamming_distance};

inline std::function<float(const Quaternion *, const Quaternion *,
                           const std::array<bool, JOINT_COUNT> &selectedJ)> quaternion_dist = quat_dist;

inline std::function<float(const Quaternion *, const Quaternion *,
                           const std::array<float, JOINT_COUNT> &selectedJ)> quaternion_dist_weighted = quat_dist_weighted;

inline std::function<float(const Vec3D *, const Vec3D *)> vector_dist = vec_dist;

#endif //DISTANCEMEASURESHMT_DISTANCE_FUNCTIONS_H
