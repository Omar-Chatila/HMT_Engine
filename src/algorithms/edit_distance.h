#ifndef DISTANCEMEASURES_HMT_EDIT_DISTANCE_H
#define DISTANCEMEASURES_HMT_EDIT_DISTANCE_H

#include <vector>
#include <fstream>
#include <sstream>
#include <functional>
#include "distance_measures.h"

using std::pair, std::vector;

class EditDistance {

private:
    static bool
    match(Vec3D &vec1, Vec3D &vec2, float epsilon, std::function<float(const Vec3D &, const Vec3D &)> &func);

    static bool
    match(Vec3D *vec1, Vec3D *vec2, float epsilon, std::function<float(const Vec3D *, const Vec3D *)> &func);

    static bool
    match(Quaternion *vec1, Quaternion *vec2, float epsilon, std::function<float(const Quaternion *, const Quaternion *,
                                                                                 const std::array<bool, JOINT_COUNT> &selectedJ)> &func);

    static bool
    match(Quaternion *vec1, Quaternion *vec2, float epsilon, std::function<float(const Quaternion *, const Quaternion *,
                                                                                 const std::array<float, JOINT_COUNT> &selectedJ)> &func);

public:
    // Edit distance on real sequences
    static int edr(Vec3D *v1, Vec3D *v2, int size_v1, int size_v2, float epsilon,
                   std::function<float(const Vec3D &, const Vec3D &)> func);

    static int edr(const std::vector<Vec3D *> &inpF, const std::vector<Vec3D *> &refF, float epsilon,
                   std::function<float(const Vec3D *, const Vec3D *)> func);

    static int edr(const std::vector<Quaternion *> &inpF, const std::vector<Quaternion *> &refF, float epsilon,
                   std::function<float(const Quaternion *, const Quaternion *,
                                       const std::array<bool, JOINT_COUNT> &selectedJ)> &func);

    static int edr(const std::vector<Quaternion *> &inpF, const std::vector<Quaternion *> &refF, float epsilon,
                   std::function<float(const Quaternion *, const Quaternion *,
                                       const std::array<float, JOINT_COUNT> &selectedJ)> &func);

    // lambda := Penalty for deletion operation
    // nu     := Elasticity parameter - nu >=0 needed for distance measure
    static float
    twed(const std::vector<Quaternion *> &inpF, const std::vector<Quaternion *> &refF, float nu, float lambda,
         std::function<float(const Quaternion *, const Quaternion *,
                             const std::array<bool, JOINT_COUNT> &selectedJ)> &func);

    static float
    twed(const std::vector<Quaternion *> &inpF, const std::vector<Quaternion *> &refF, float nu, float lambda,
         std::function<float(const Quaternion *, const Quaternion *,
                             const std::array<float, JOINT_COUNT> &selectedJ)> &func);
};

#endif