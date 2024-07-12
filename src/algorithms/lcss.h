#ifndef DISTANCEMEASURES_HMT_LCSS_H
#define DISTANCEMEASURES_HMT_LCSS_H

#include <vector>
#include <fstream>
#include <sstream>
#include <functional>
#include "distance_measures.h"


class LCSS {

private:
    static bool match(Vec3D &vec1, Vec3D &vec2, float epsilon, std::function<float(const Vec3D &, const Vec3D &)> func);

    static bool match(Vec3D *vec1, Vec3D *vec2, float epsilon, std::function<float(const Vec3D *, const Vec3D *)> func);

    static bool
    match(Quaternion *vec1, Quaternion *vec2, float epsilon, std::function<float(const Quaternion *, const Quaternion *,
                                                                                 const std::array<bool, JOINT_COUNT> &selectedJ)> &func);

public:
    static float lcss(const std::vector<Vec3D *> &inpF, const std::vector<Vec3D *> &refF, float epsilon,
                      std::function<float(const Vec3D *, const Vec3D *)> func);

    static float
    lcss(const std::vector<Quaternion *> &inpF, const std::vector<Quaternion *> &refF, float epsilon, float delta,
         std::function<float(const Quaternion *, const Quaternion *,
                             const std::array<bool, JOINT_COUNT> &selectedJ)> &func);

    static float
    lcss(const std::vector<Quaternion *> &inpF, const std::vector<Quaternion *> &refF, float epsilon, float delta,
         std::function<float(const Quaternion *, const Quaternion *,
                             const std::array<float, JOINT_COUNT> &jWeights)> &func);
};


#endif //DISTANCEMEASURES_HMT_LCSS_H
