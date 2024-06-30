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
                         float lambda, std::function<float(const Quaternion *, const Quaternion *)> dist) {
    size_t arr2_lgt = refF.size();
    size_t arr1_lgt = inpF.size();
    float* D = (float*) calloc(arr1_lgt * arr2_lgt, sizeof(float));
    for(unsigned int i = 1; i < arr1_lgt; i++) {
        D[INDEX(i, 0)] = std::numeric_limits<float>::infinity();
    }
    for(unsigned int i = 1; i < arr2_lgt; i++) {
        D[INDEX(0, i)] = std::numeric_limits<float>::infinity();
    }

    for(unsigned int j = 1; j < arr2_lgt; j++) {
        for(unsigned int i = 1; i < arr1_lgt; i++) {
            float del_a = D[INDEX(i - 1, j)]
                          + dist(inpF[i - 1], inpF[i])
                          + nu
                          + lambda;
            float del_b = D[INDEX(i, j - 1)]
                          + dist(refF[j - 1], refF[j])
                          + nu
                          + lambda;
            float match = D[INDEX(i-1, j-1)]
                          + dist(inpF[i], refF[j])
                          + dist(inpF[i - 1], refF[j - 1])
                          + nu * (std::fabs(i - j) + std::fabs(i - 1 - (j - 1)));

            D[INDEX(i, j)] = fmin(match, fmin(del_a, del_b));
        }
    }
    double ret = D[INDEX(arr1_lgt - 1, arr2_lgt - 1)];
    free(D);
    return ret;
}