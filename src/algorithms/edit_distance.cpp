#include "edit_distance.h"
#include "defines.h"

bool EditDistance::match(Vec3D &vec1, Vec3D &vec2, float epsilon, std::function<float(const Vec3D&, const Vec3D&)> func) {
    return func(vec1, vec2) < epsilon;
}

float EditDistance::edr(Vec3D *v1, Vec3D *v2, int size_v1, int size_v2, float epsilon, std::function<float(const Vec3D&, const Vec3D&)> func) {
    const int n = size_v1;
    const int m = size_v2;

    auto *S = new int[(n + 1) * (m + 1)];
    S[0] = 0;
    for (int i = 1; i <= n; ++i) {
        S[i * (m + 1)] = i;
    }
    for (int j = 1; j <= m; ++j) {
        S[j] = j;
    }

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            int subcost = 1;
            if (match(v1[i - 1], v2[j - 1], 3, func)) {
                subcost = 0;
            }
            S[i * (m + 1) + j] = subcost 
                + std::min({S[(i-1) * (m + 1) + j], S[i * (m + 1) + (j-1)], S[(i-1) * (m + 1) + (j-1)]});
        }
    }

    std::ofstream outfile("output.txt");

    for (int i = 0; i <= n; ++i) {
        for (int j = 0; j <= m; ++j) {
            outfile << ", " << S[i * (m + 1) + j];
        }
        outfile << std::endl;
    }
    float result = (n + 1) * (m + 1) - 1;
    free(S);
    return result;
}