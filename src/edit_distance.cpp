#include "edit_distance.h"

bool EditDistance::match(Vec3D &vec1, Vec3D &vec2, float epsilon, std::function<double(const Vec3D&, const Vec3D&)> func) {
    return func(vec1, vec2) < epsilon;
}

double EditDistance::edr(Vec3D *v1, Vec3D *v2, int size_v1, int size_v2, double epsilon, std::function<double(const Vec3D&, const Vec3D&)> func) {
    const int n = size_v1 + 1;
    const int m = size_v2 + 1;

    auto *matrix = new double[n * m];
    for (int i = 0; i < m; i++) {
        matrix[i] = i;
    }
    for (int i = 0; i < n; i++) {
        matrix[m * i] = i;
    }

    matrix[0] = 0;
    for (int i = 1; i <= size_v1; i++) {
        for (int j = 1; j <= size_v2; j++) {
            int subcost = 1;
            if (match(v1[i - 1], v2[j - 1], 3, func)) {
                subcost = 0;
            }
            int index = i * (size_v2 + 1) + j;
            matrix[i * m + j] = subcost + std::min(matrix[index - m],
                                                std::min(matrix[index - 1],
                                                         matrix[index - m - 1]));
        }
    }
    return matrix[n * m - 1];
}