#include "edit_distance.h"
#include "defines.h"

bool EditDistance::match(Vec3D &vec1, Vec3D &vec2, float epsilon, std::function<float(const Vec3D&, const Vec3D&)> func) {
    return func(vec1, vec2) < epsilon;
}

bool EditDistance::match(Vec3D *vec1, Vec3D *vec2, float epsilon, std::function<float(const Vec3D*, const Vec3D*)> func) {
    return func(vec1, vec2) < epsilon;
}

bool EditDistance::match(Quaternion *vec1, Quaternion *vec2, float epsilon, std::function<float(const Quaternion*, const Quaternion*)> func) {
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
            if (match(v1[i - 1], v2[j - 1], epsilon, func)) {
                subcost = 0;
            }
            S[current] = subcost
                + std::min({S[above], S[left], S[diag_left]});
        }
    }
    float result = S[(n + 1) * (m + 1) - 1];
    delete[] S;
    return result;
}

float EditDistance::edr(const std::vector<Vec3D *> &inpF, const std::vector<Vec3D *> &refF, float epsilon, std::function<float(const Vec3D*, const Vec3D*)> func) {
    const int n = inpF.size();
    const int m = refF.size();
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
            if (match(inpF[i - 1], refF[j - 1], epsilon, func)) {
                subcost = 0;
            }
            S[current] = subcost
                         + std::min({S[above], S[left], S[diag_left]});
        }
    }

    float result = S[(n + 1) * (m + 1) - 1];
    delete[] S;
    return result;
}

float EditDistance::edr(const std::vector<Quaternion *> &inpF, const std::vector<Quaternion *> &refF, float epsilon, std::function<float(const Quaternion*, const Quaternion*)> func) {
    const int n = inpF.size();
    const int m = refF.size();
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
            if (match(inpF[i - 1], refF[j - 1], epsilon, func)) {
                subcost = 0;
            }
            S[current] = subcost
                         + std::min({S[above], S[left], S[diag_left]});
        }
    }

    float result = S[(n + 1) * (m + 1) - 1];
    delete[] S;
    return result;
}