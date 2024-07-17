#include "edit_distance.h"
#include "defines.h"
#include "algo_settings.h"

#define JOINT_WEIGHTS AlgoSettings::getInstance().joint_weights
#define JOINT_SELECTION AlgoSettings::getInstance().selected_joints

bool
EditDistance::match(Vec3D &vec1, Vec3D &vec2, float epsilon, std::function<float(const Vec3D &, const Vec3D &)> &func) {
    return func(vec1, vec2) < epsilon;
}

bool
EditDistance::match(Vec3D *vec1, Vec3D *vec2, float epsilon, std::function<float(const Vec3D *, const Vec3D *)> &func) {
    return func(vec1, vec2) < epsilon;
}

bool EditDistance::match(Quaternion *vec1, Quaternion *vec2, float epsilon,
                         std::function<float(const Quaternion *, const Quaternion *,
                                             const std::array<bool, JOINT_COUNT> &selectedJ)> &func) {
    return func(vec1, vec2, JOINT_SELECTION) < epsilon;
}

bool EditDistance::match(Quaternion *vec1, Quaternion *vec2, float epsilon,
                         std::function<float(const Quaternion *, const Quaternion *,
                                             const std::array<float, JOINT_COUNT> &selectedJ)> &func) {
    return func(vec1, vec2, JOINT_WEIGHTS) < epsilon;
}

int EditDistance::edr(Vec3D *v1, Vec3D *v2, int size_v1, int size_v2, float epsilon,
                      std::function<float(const Vec3D &, const Vec3D &)> func) {
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
    int result = S[(n + 1) * (m + 1) - 1];
    delete[] S;
    return result;
}

int EditDistance::edr(const std::vector<Vec3D *> &inpF, const std::vector<Vec3D *> &refF, float epsilon,
                      std::function<float(const Vec3D *, const Vec3D *)> func) {
    const size_t n = inpF.size();
    const size_t m = refF.size();
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

    int result = S[(n + 1) * (m + 1) - 1];
    delete[] S;
    return result;
}

int EditDistance::edr(const std::vector<Quaternion *> &inpF, const std::vector<Quaternion *> &refF, float epsilon,
                      std::function<float(const Quaternion *, const Quaternion *,
                                          const std::array<bool, JOINT_COUNT> &selectedJ)> &func) {
    const size_t n = inpF.size();
    const size_t m = refF.size();
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

    int result = S[(n + 1) * (m + 1) - 1];
    delete[] S;
    return result;
}

int EditDistance::edr(const std::vector<Quaternion *> &inpF, const std::vector<Quaternion *> &refF, float epsilon,
                      std::function<float(const Quaternion *, const Quaternion *,
                                          const std::array<float, JOINT_COUNT> &selectedJ)> &func) {
    const size_t n = inpF.size();
    const size_t m = refF.size();
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

    int result = S[(n + 1) * (m + 1) - 1];
    delete[] S;
    return result;
}

#define INDEX(x, y) ((y)*arr1_lgt + (x))

/*
Please cite as:
@article{Marteau:2009:TWED,
author = {Marteau, Pierre-Francois},
        title = {Time Warp Edit Distance with Stiffness Adjustment for Time Series Matching},
        journal = {IEEE Trans. Pattern Anal. Mach. Intell.},
        issue_date = {February 2009},
        volume = {31},
        number = {2},
        month = feb,
        year = {2009},
        issn = {0162-8828},
        pages = {306--318},
        numpages = {13},
        url = {http://dx.doi.org/10.1109/TPAMI.2008.76},
doi = {10.1109/TPAMI.2008.76},
        acmid = {1496043},
        publisher = {IEEE Computer Society},
        address = {Washington, DC, USA},
keywords = {Dynamic programming, Pattern recognition, Pattern recognition, time series, algorithms, similarity measures., Similarity measures, algorithms, similarity measures., time series},
}
*/
float EditDistance::twed(const std::vector<Quaternion *> &inpF, const std::vector<Quaternion *> &refF, float nu,
                         float lambda, std::function<float(const Quaternion *, const Quaternion *,
                                                           const std::array<bool, JOINT_COUNT> &selectedJ)> &func) {
    size_t arr2_lgt = refF.size();
    size_t arr1_lgt = inpF.size();
    auto *D = (float *) calloc(arr1_lgt * arr2_lgt, sizeof(float));
    for (unsigned int i = 1; i < arr1_lgt; i++) {
        D[INDEX(i, 0)] = std::numeric_limits<float>::infinity();
    }
    for (unsigned int i = 1; i < arr2_lgt; i++) {
        D[INDEX(0, i)] = std::numeric_limits<float>::infinity();
    }

    for (unsigned int j = 1; j < arr2_lgt; j++) {
        for (unsigned int i = 1; i < arr1_lgt; i++) {
            float del_a = D[INDEX(i - 1, j)]
                          + func(inpF[i - 1], inpF[i], JOINT_SELECTION)
                          + nu
                          + lambda;
            float del_b = D[INDEX(i, j - 1)]
                          + func(refF[j - 1], refF[j], JOINT_SELECTION)
                          + nu
                          + lambda;
            float match = D[INDEX(i - 1, j - 1)]
                          + func(inpF[i], refF[j], JOINT_SELECTION)
                          + func(inpF[i - 1], refF[j - 1], JOINT_SELECTION)
                          + nu * (std::fabs(i - j) + std::fabs(i - 1 - (j - 1)));

            D[INDEX(i, j)] = std::min({match, del_a, del_b});
        }
    }
    float ret = D[INDEX(arr1_lgt - 1, arr2_lgt - 1)];
    free(D);
    return ret;
}

float EditDistance::twed(const std::vector<Quaternion *> &inpF, const std::vector<Quaternion *> &refF, float nu,
                         float lambda, std::function<float(const Quaternion *, const Quaternion *,
                                                           const std::array<float, JOINT_COUNT> &selectedJ)> &func) {
    size_t arr2_lgt = refF.size();
    size_t arr1_lgt = inpF.size();
    auto *D = (float *) calloc(arr1_lgt * arr2_lgt, sizeof(float));
    for (unsigned int i = 1; i < arr1_lgt; i++) {
        D[INDEX(i, 0)] = std::numeric_limits<float>::infinity();
    }
    for (unsigned int i = 1; i < arr2_lgt; i++) {
        D[INDEX(0, i)] = std::numeric_limits<float>::infinity();
    }

    for (unsigned int j = 1; j < arr2_lgt; j++) {
        for (unsigned int i = 1; i < arr1_lgt; i++) {
            float del_a = D[INDEX(i - 1, j)]
                          + func(inpF[i - 1], inpF[i], JOINT_WEIGHTS)
                          + nu
                          + lambda;
            float del_b = D[INDEX(i, j - 1)]
                          + func(refF[j - 1], refF[j], JOINT_WEIGHTS)
                          + nu
                          + lambda;
            float match = D[INDEX(i - 1, j - 1)]
                          + func(inpF[i], refF[j], JOINT_WEIGHTS)
                          + func(inpF[i - 1], refF[j - 1], JOINT_WEIGHTS)
                          + nu * (std::fabs(i - j) + std::fabs(i - 1 - (j - 1)));

            D[INDEX(i, j)] = std::min({match, del_a, del_b});
        }
    }
    float ret = D[INDEX(arr1_lgt - 1, arr2_lgt - 1)];
    free(D);
    return ret;
}