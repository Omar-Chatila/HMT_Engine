#ifndef DISTANCEMEASURES_HMT_FRECHET_H
#define DISTANCEMEASURES_HMT_FRECHET_H

#include <vector>
#include <functional>
#include "distance_measures.h"

class Frechet {
private:

public:
    static float
    frechet(Vec3D *v1, Vec3D *v2, int size_v1, int size_v2, std::function<float(const Vec3D &, const Vec3D &)> func);

    static float frechet(const std::vector<Vec3D *> &inpF, const std::vector<Vec3D *> &refF,
                         std::function<float(const Vec3D *, const Vec3D *)> func);

    static float frechet(const std::vector<Quaternion *> &inpF, const std::vector<Quaternion *> &refF,
                         std::function<float(const Quaternion *, const Quaternion *,
                                             const std::array<bool, JOINT_COUNT> &selectedJ)> &func);

    static float frechet(const std::vector<Quaternion *> &inpF, const std::vector<Quaternion *> &refF,
                         std::function<float(const Quaternion *, const Quaternion *,
                                             const std::array<float, JOINT_COUNT> &jWeights)> &func);
};


#endif //DISTANCEMEASURES_HMT_FRECHET_H
