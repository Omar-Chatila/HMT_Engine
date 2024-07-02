#include "lcss.h"
#include "algo_settings.h"
#include <cmath>

bool LCSS::match(Vec3D &vec1, Vec3D &vec2, float epsilon, std::function<float(const Vec3D &, const Vec3D &)> func) {
    return func(vec1, vec2) < epsilon;
}

bool LCSS::match(Vec3D *vec1, Vec3D *vec2, float epsilon, std::function<float(const Vec3D *, const Vec3D *)> func) {
    return func(vec1, vec2) < epsilon;
}

bool LCSS::match(Quaternion *vec1, Quaternion *vec2, float epsilon,
                 std::function<float(const Quaternion *, const Quaternion *,
                                     const std::array<bool, JOINT_COUNT> &selectedJ)> &func) {
    return func(vec1, vec2, AlgoSettings::getInstance().selected_joints) < epsilon;
}

#define INDEX(x, y) ((x)* (m + 1) + (y))

float
LCSS::lcss(const std::vector<Quaternion *> &inpF, const std::vector<Quaternion *> &refF, float epsilon, float delta,
           std::function<float(const Quaternion *, const Quaternion *,
                               const std::array<bool, JOINT_COUNT> &selectedJ)> &func) {
    size_t n = inpF.size();
    size_t m = refF.size();
    auto *L = new float[(n + 1) * (m + 1)]();
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            if (static_cast<float>(std::abs(i - j)) <= delta &&
                func(inpF[i - 1], refF[j - 1], AlgoSettings::getInstance().selected_joints) < epsilon) {
                L[INDEX(i, j)] = 1 + L[INDEX(i - 1, j - 1)];
            } else {
                L[INDEX(i, j)] = std::max(L[INDEX(i - 1, j)], L[INDEX(i, j - 1)]);
            }
        }
    }
    float result = L[INDEX(n, m)];
    delete[] L;
    return result;
}