#include <cmath>
#include <algorithm>
#include "dtw.h"

float *Dtw::dtw(const Vec3D *v1, const Vec3D *v2, int size_v1, int size_v2,
                std::function<float(const Vec3D &, const Vec3D &)> &func) {
    const int n = size_v1;
    const int m = size_v2;

    float *S = (float *) calloc((n + 1) * (m + 1), sizeof(float));
    S[0] = 0;
    for (int i = 1; i <= n; ++i) {
        S[i * (m + 1)] = std::numeric_limits<float>::infinity();
    }
    for (int j = 1; j <= m; ++j) {
        S[j] = std::numeric_limits<float>::infinity();
    }

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            float cost = func(v1[i - 1], v2[j - 1]);
            S[CURRENT_INDEX] = cost + std::min({S[ABOVE_INDEX], S[LEFT_INDEX], S[DIAG_LEFT_INDEX]});
        }
    }
    return S;
}

float *Dtw::dtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                std::function<float(const Quaternion *, const Quaternion *)> &func) {
    const int n = inp_traj.size();
    const int m = ref_traj.size();

    float *S = (float *) (malloc((n + 1) * (m + 1) * sizeof(float)));
    S[0] = 0;
    for (int i = 1; i <= n; ++i) {
        S[i * (m + 1)] = std::numeric_limits<float>::infinity();
    }
    for (int j = 1; j <= m; ++j) {
        S[j] = std::numeric_limits<float>::infinity();
    }

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            float cost = func(inp_traj[i - 1], ref_traj[j - 1]);
            S[CURRENT_INDEX] = cost + std::min({S[ABOVE_INDEX], S[LEFT_INDEX], S[DIAG_LEFT_INDEX]});
        }
    }
    return S;
}

float MLWF(int i, int j, float g, float w_max, int m_c) {
    return w_max / (1 + expf(-g * (std::abs(i - j) - m_c)));;
}

float *Dtw::wdtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                 std::function<float(const Quaternion *, const Quaternion *)> &func, float g, float w_max) {
    const int n = inp_traj.size();
    const int m = ref_traj.size();
    const int m_c = (n + m) / 2;
    auto *S = (float *) (malloc((n + 1) * (m + 1) * sizeof(float)));
    S[0] = 0;
    for (int i = 1; i <= n; ++i) {
        S[i * (m + 1)] = std::numeric_limits<float>::infinity();
    }
    for (int j = 1; j <= m; ++j) {
        S[j] = std::numeric_limits<float>::infinity();
    }

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            float cost = MLWF(i, j, g, w_max, m_c) * func(inp_traj[i - 1], ref_traj[j - 1]);
            S[CURRENT_INDEX] = cost + std::min({S[ABOVE_INDEX], S[LEFT_INDEX], S[DIAG_LEFT_INDEX]});
        }
    }
    return S;
}

void derivative_transform(const std::vector<Quaternion *> traj) {
    for (size_t i = 1; i < traj.size() - 1; ++i) {
        for (size_t j = 0; j < JOINT_COUNT; ++j) {
            traj[i][j] = ((traj[i][j] - traj[i - 1][j]) +
                          ((traj[i + 1][j] - traj[i - 1][j]) / 2.0f)) / 2.0f;
        }
    }
    // Copy first and last frames as is
    for (int i = 0; i < JOINT_COUNT; i++) {
        traj[0][i] = traj[1][i];
    }
    for (int i = 0; i < JOINT_COUNT; i++) {
        traj[traj.size() - 1][i] = traj[traj.size() - 2][i];
    }
}

float *Dtw::wddtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                  std::function<float(const Quaternion *, const Quaternion *)> &func, float g, float w_max) {
    const int n = inp_traj.size();
    const int m = ref_traj.size();
    const int m_c = (n + m) / 2;

    // Compute the derivative transformation
    derivative_transform(inp_traj);
    derivative_transform(ref_traj);

    auto *S = (float *) (malloc((n + 1) * (m + 1) * sizeof(float)));
    S[0] = 0;
    for (int i = 1; i <= n; ++i) {
        S[i * (m + 1)] = std::numeric_limits<float>::infinity();
    }
    for (int j = 1; j <= m; ++j) {
        S[j] = std::numeric_limits<float>::infinity();
    }

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            float weight = MLWF(i, j, g, w_max, m_c);
            float cost = weight * func(inp_traj[i - 1], ref_traj[j - 1]);
            S[i * (m + 1) + j] =
                    cost + std::min({S[ABOVE_INDEX], S[LEFT_INDEX], S[DIAG_LEFT_INDEX]});
        }
    }
    return S;
}

std::pair<float, std::vector<int>> Dtw::get_cost_and_alignment(float *cost_matrix, int n, int m) {
    int index = (m + 1) * (n + 1) - 1;
    std::vector<int> alignment;
    const float cost = cost_matrix[index];

    while (index > 0) {
        alignment.push_back(index);
        int next_index;
        if (cost_matrix[index - (m + 2)] < cost_matrix[index - 1] &&
            cost_matrix[index - (m + 2)] < cost_matrix[index - m - 1]) {
            next_index = index - (m + 2); // Move diagonally up-left
        } else if (cost_matrix[index - 1] < cost_matrix[index - m - 1]) {
            next_index = index - 1; // Move left
        } else {
            next_index = index - m - 1; // Move up
        }
        index = next_index; // Move to the next index
    }
    std::reverse(alignment.begin(), alignment.end());
    return {cost, alignment};
}

float *Dtw::get_cost_matrix(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                            std::function<float(const Quaternion *, const Quaternion *)> &func) {
    const int n = inp_traj.size();
    const int m = ref_traj.size();

    float *S = (float *) calloc((n + 1) * (m + 1), sizeof(float));
    S[0] = 0;
    for (int i = 1; i <= n; ++i) {
        S[i * (m + 1)] = std::numeric_limits<float>::infinity();
    }
    for (int j = 1; j <= m; ++j) {
        S[j] = std::numeric_limits<float>::infinity();
    }

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            S[CURRENT_INDEX] = func(inp_traj[i - 1], ref_traj[j - 1]);
        }
    }
    return S;
}

