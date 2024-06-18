#include "lcss.h"
#include <cmath>

bool LCSS::match(Vec3D &vec1, Vec3D &vec2, float epsilon, std::function<float(const Vec3D &, const Vec3D &)> func) {
    return func(vec1, vec2) < epsilon;
}

bool LCSS::match(Vec3D *vec1, Vec3D *vec2, float epsilon, std::function<float(const Vec3D *, const Vec3D *)> func) {
    return func(vec1, vec2) < epsilon;
}

bool LCSS::match(Quaternion *vec1, Quaternion *vec2, float epsilon,
                 std::function<float(const Quaternion *, const Quaternion *)> func) {
    return func(vec1, vec2) < epsilon;
}

/*Algorithm LCSSδ,ε(TA, TB, δ, ε)
Input: Trajectories TA and TB of lengths m and k, integers δ and ε
        Output: LCSS value

1. Initialize a (m+1) x (k+1) matrix LCSS with zeros

2. For i from 1 to m:
a. For j from 1 to k:
i. If |i - j| ≤ δ:
- If |TA[i] - TB[j]| ≤ ε:
LCSS[i][j] = 1 + LCSS[i-1][j-1]
- Else:
LCSS[i][j] = max(LCSS[i-1][j], LCSS[i][j-1])

3. Return LCSS[m][k]
 */
#define INDEX(x, y) ((x)* (m + 1) + (y))

float  LCSS::lcss(const std::vector<Quaternion *> &inpF, const std::vector<Quaternion *> &refF, float epsilon, float delta,
                  std::function<float(const Quaternion *, const Quaternion *)> func) {
    size_t n = inpF.size();
    size_t m = refF.size();
    float* L = new float[(n + 1) * (m + 1)]();
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            if (std::abs(i - j) <= delta && func(inpF[i - 1], refF[j - 1]) < epsilon) {
                L[INDEX(i, j)] = 1 + L[INDEX(i - 1, j - 1)];
            } else {
                L[INDEX(i, j)] = std::max(L[INDEX(i - 1,j)], L[INDEX(i, j - 1)]);
            }
        }
    }
    float result = L[INDEX(n, m)];
    delete[] L;
    return result;
}