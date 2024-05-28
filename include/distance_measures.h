#ifndef DISTANCEMEASURES_HMT_DISTANCE_MEASURES_H
#define DISTANCEMEASURES_HMT_DISTANCE_MEASURES_H

#include <cmath>
#include "util.h"

inline double squared_euclid(double a, double b) {
    return std::pow(b - a, 2);
}

inline double diff(double a, double b) {
    return b - a;
}

inline double abs(double a, double b) {
    return std::abs(a - b);
}

inline double abs(const Vec3D &a) {
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

inline double euclid(const Vec3D &a, const Vec3D &b) {
    return sqrt(pow(a.x - b.x, 2) +pow(a.y - b.y, 2) +pow(a.z - b.z, 2));
}

inline double squared_euclid(const Vec3D &a, const Vec3D &b) {
    return pow(a.x - b.x, 2) +pow(a.y - b.y, 2) +pow(a.z - b.z, 2);
}

inline double manhattan(const Vec3D &a, const Vec3D &b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y) + std::abs(a.z - b.z);
}

inline double minkowski(const Vec3D &a, const Vec3D &b, int p) {
    return pow(std::abs(pow(a.x - b.x, p) +
                                std::abs(pow(a.y -b.y, p)) +
                                std::abs(pow(a.z -b.z, p))), static_cast<double>(1.0 / p));
}

inline double maximum_norm(const Vec3D &a, const Vec3D &b) {
    return std::max(std::abs(a.x - b.x), std::max(std::abs(a.y - b.y), std::abs(a.z - b.z)));
}

inline double minimum_distance(const Vec3D &a, const Vec3D &b) {
    return std::min(std::abs(a.x - b.x), std::min(std::abs(a.y - b.y), std::abs(a.z - b.z)));
}

inline double canberra_distance(const Vec3D &a, const Vec3D &b) {
    return std::abs(a.x - b.x) / (std::abs(a.x) + std::abs(b.x)) +
            std::abs(a.y - b.y) / (std::abs(a.y) + std::abs(b.y)) +
            std::abs(a.z - b.z) / (std::abs(a.z) + std::abs(b.z));
}

inline double scalar_product(const Vec3D &a, const Vec3D &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline double cosine_similarity(const Vec3D &a, const Vec3D &b) {
    return scalar_product(a, b) / (abs(a) * abs(b));
}

inline double cosine_distance1(const Vec3D &a, const Vec3D &b) {
    return 1 - cosine_similarity(a, b);
}

inline double cosine_distance2(const Vec3D &a, const Vec3D &b) {
    return acos(cosine_similarity(a, b));
}

inline double hamming_distance(const Vec3D &a, const Vec3D &b) {
    return a.x == b.x + a.y == b.y + a.z == b.z;
}

#endif //DISTANCEMEASURES_HMT_DISTANCE_MEASURES_H
