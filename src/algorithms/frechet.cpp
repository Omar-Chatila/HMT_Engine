#include "frechet.h"

#define INDEX(x, y) ((x)* (m) + (y))

float Frechet::frechet(const std::vector<Quaternion *> &inpF, const std::vector<Quaternion *> &refF,
                       std::function<float(const Quaternion *, const Quaternion *)> func) {
    size_t n = inpF.size();
    size_t m = refF.size();
    float *C = new float[n * m]();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            float d = func(inpF[i], refF[j]);
            if (i > 0 && j > 0)
                C[INDEX(i, j)] = std::max(
                        std::min(C[INDEX(i - 1, j)], std::min(C[INDEX(i - 1, j - 1)], C[INDEX(i, j - 1)])), d);
            else if (i > 0 && j == 0)
                C[INDEX(i, j)] = std::max(C[INDEX(i - 1, 0)], d);
            else if (i == 0 && j > 0)
                C[INDEX(i, j)] = std::max(C[INDEX(0, j - 1)], d);
            else
                C[INDEX(i, j)] = d;
        }
    }
    float result = C[(n) * (m) - 1];
    delete[] C;
    return result;
}

float Frechet::frechet(const std::vector<Vec3D *> &inpF, const std::vector<Vec3D *> &refF,
                       std::function<float(const Vec3D *, const Vec3D *)> func) {
    size_t n = inpF.size();
    size_t m = refF.size();
    float *C = new float[n * m]();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            float d = func(inpF[i], refF[j]);
            if (i > 0 && j > 0)
                C[INDEX(i, j)] = std::max(
                        std::min(C[INDEX(i - 1, j)], std::min(C[INDEX(i - 1, j - 1)], C[INDEX(i, j - 1)])), d);
            else if (i > 0 && j == 0)
                C[INDEX(i, j)] = std::max(C[INDEX(i - 1, 0)], d);
            else if (i == 0 && j > 0)
                C[INDEX(i, j)] = std::max(C[INDEX(0, j - 1)], d);
            else
                C[INDEX(i, j)] = d;
        }
    }
    float result = C[(n) * (m) - 1];
    delete[] C;
    return result;
}

