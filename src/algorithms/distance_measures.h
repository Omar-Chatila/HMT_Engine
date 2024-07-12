#ifndef DISTANCEMEASURES_HMT_DISTANCE_MEASURES_H
#define DISTANCEMEASURES_HMT_DISTANCE_MEASURES_H

#include <cmath>
#include "util.h"

enum Distances {
    QUATERNION, QUATERNION_WEIGHTED, EUCLID, SQUARED_EUCLID, MANHATTAN, MINKOWSKI, MAX_NORM, MIN_DISTANCE,
    CANBERRA_DISTANCE, SCALAR_PRODUCT, COSINE_SIMILARITY, COSINE_DISTANCE1,
    COSINE_DISTANCE2, HAMMING_DISTANCE
};

inline float abs(const Vec3D &a) {
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

inline float euclid(const Vec3D &a, const Vec3D &b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
}

inline float squared_euclid(const Vec3D &a, const Vec3D &b) {
    return pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2);
}

inline float manhattan(const Vec3D &a, const Vec3D &b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y) + std::abs(a.z - b.z);
}

inline float minkowski(const Vec3D &a, const Vec3D &b, float p = 2) {
    return std::pow(
            std::pow(std::abs(a.x - b.x), p) + std::pow(std::abs(a.y - b.y), p) + std::pow(std::abs(a.z - b.z), p),
            1 / p);
}

inline float maximum_norm(const Vec3D &a, const Vec3D &b) {
    return std::max(std::abs(a.x - b.x), std::max(std::abs(a.y - b.y), std::abs(a.z - b.z)));
}

inline float minimum_distance(const Vec3D &a, const Vec3D &b) {
    return std::min(std::abs(a.x - b.x), std::min(std::abs(a.y - b.y), std::abs(a.z - b.z)));
}

inline float canberra_distance(const Vec3D &a, const Vec3D &b) {
    return std::abs(a.x - b.x) / (std::abs(a.x) + std::abs(b.x)) +
           std::abs(a.y - b.y) / (std::abs(a.y) + std::abs(b.y)) +
           std::abs(a.z - b.z) / (std::abs(a.z) + std::abs(b.z));
}

inline float scalar_product(const Vec3D &a, const Vec3D &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline float cosine_similarity(const Vec3D &a, const Vec3D &b) {
    return scalar_product(a, b) / (abs(a) * abs(b));
}

inline float cosine_distance1(const Vec3D &a, const Vec3D &b) {
    return 1 - cosine_similarity(a, b);
}

inline float cosine_distance2(const Vec3D &a, const Vec3D &b) {
    return std::acos(cosine_similarity(a, b));
}

inline float hamming_distance(const Vec3D &a, const Vec3D &b) {
    return a.x == b.x + a.y == b.y + a.z == b.z;
}

// Metrics for 3D Rotations: Comparison and Analysis Du Q. Huynh: phi_4 Formel 20
inline float
quat_dist(const Quaternion *first, const Quaternion *second, const std::array<bool, JOINT_COUNT> selectedJoints) {
    float distance = 0.0f;
    for (int i = 0; i < JOINT_COUNT; i++) {
        if (selectedJoints[i])
            distance += 1 - std::abs(first[i] * second[i]);
    }
    return distance;
}

inline float
quat_dist_weighted(const Quaternion *first, const Quaternion *second,
                   const std::array<float, JOINT_COUNT> joint_weights) {
    float distance = 0.0f;
    for (int i = 0; i < JOINT_COUNT; i++) {
        distance += joint_weights[i] * (1 - std::abs(first[i] * second[i]));
    }
    return distance;
}

// Accumulate euclid distance over all Joints
inline float vec_dist(const Vec3D *first, const Vec3D *second) {
    float distance = 0.0f;
    for (int i = 0; i < JOINT_COUNT; i++) {
        distance += euclid(first[i], second[i]);
    }
    return distance;
}

#endif
