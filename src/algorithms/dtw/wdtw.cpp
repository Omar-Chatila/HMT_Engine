//
// Created by omarc on 17/07/2024.
//
#include "dtw.h"

float MLWF(int i, int j, float g, float w_max, int m_c) {
    return w_max / (1 + expf(-g * (static_cast<float>(std::abs(i - j) - m_c))));
}

float *Dtw::wdtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                 std::function<float(const Quaternion *, const Quaternion *,
                                     const std::array<bool, JOINT_COUNT> &selectedJ)> &func, float g, float w_max) {
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
            float cost = MLWF(i, j, g, w_max, m_c) *
                         func(inp_traj[i - 1], ref_traj[j - 1], AlgoSettings::getInstance().selected_joints);
            S[CURRENT_INDEX] = cost + std::min({S[ABOVE_INDEX], S[LEFT_INDEX], S[DIAG_LEFT_INDEX]});
        }
    }
    return S;
}

float *Dtw::wdtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                 std::function<float(const Quaternion *, const Quaternion *,
                                     const std::array<float, JOINT_COUNT> &selectedJ)> &func, float g, float w_max) {
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
            float cost = MLWF(i, j, g, w_max, m_c) *
                         func(inp_traj[i - 1], ref_traj[j - 1], AlgoSettings::getInstance().joint_weights);
            S[CURRENT_INDEX] = cost + std::min({S[ABOVE_INDEX], S[LEFT_INDEX], S[DIAG_LEFT_INDEX]});
        }
    }
    return S;
}

void derivative_transform(const std::vector<Quaternion *> &trajectory) {
    for (size_t i = 1; i < trajectory.size() - 1; ++i) {
        for (size_t j = 0; j < JOINT_COUNT; ++j) {
            trajectory[i][j] = ((trajectory[i][j] - trajectory[i - 1][j]) +
                                ((trajectory[i + 1][j] - trajectory[i - 1][j]) / 2.0f)) / 2.0f;
        }
    }
    // Copy first and last frames as is
    for (int i = 0; i < JOINT_COUNT; i++) {
        trajectory[0][i] = trajectory[1][i];
    }
    for (int i = 0; i < JOINT_COUNT; i++) {
        trajectory[trajectory.size() - 1][i] = trajectory[trajectory.size() - 2][i];
    }
}

float *Dtw::wddtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                  std::function<float(const Quaternion *, const Quaternion *,
                                      const std::array<bool, JOINT_COUNT> &selectedJ)> &func, float g, float w_max) {
    std::vector<Quaternion *> inp_traj_copy = deep_copy(inp_traj);
    std::vector<Quaternion *> ref_traj_copy = deep_copy(ref_traj);

    derivative_transform(inp_traj_copy);
    derivative_transform(ref_traj_copy);

    const int n = inp_traj_copy.size();
    const int m = ref_traj_copy.size();
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
            float weight = MLWF(i, j, g, w_max, m_c);
            float cost = weight *
                         func(inp_traj_copy[i - 1], ref_traj_copy[j - 1], AlgoSettings::getInstance().selected_joints);
            S[i * (m + 1) + j] = cost + std::min({S[ABOVE_INDEX], S[LEFT_INDEX], S[DIAG_LEFT_INDEX]});
        }
    }
    return S;
}

float *Dtw::wddtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                  std::function<float(const Quaternion *, const Quaternion *,
                                      const std::array<float, JOINT_COUNT> &jWeights)> &func, float g, float w_max) {
    std::vector<Quaternion *> inp_traj_copy = deep_copy(inp_traj);
    std::vector<Quaternion *> ref_traj_copy = deep_copy(ref_traj);

    derivative_transform(inp_traj_copy);
    derivative_transform(ref_traj_copy);

    const int n = inp_traj_copy.size();
    const int m = ref_traj_copy.size();
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
            float weight = MLWF(i, j, g, w_max, m_c);
            float cost = weight *
                         func(inp_traj_copy[i - 1], ref_traj_copy[j - 1], AlgoSettings::getInstance().joint_weights);
            S[i * (m + 1) + j] = cost + std::min({S[ABOVE_INDEX], S[LEFT_INDEX], S[DIAG_LEFT_INDEX]});
        }
    }
    return S;
}

std::vector<Quaternion *> Dtw::normalize_trajectory(const std::vector<Quaternion *> &trajectory,
                                                    const std::vector<std::pair<Quaternion, Quaternion>> &max_rotations) {
    std::vector<Quaternion *> normalized_trajectory = trajectory;
    for (auto &quats: normalized_trajectory) {
        for (int i = 0; i < JOINT_COUNT; ++i) {
            quats[i].w =
                    (quats[i].w - max_rotations[i].first.w) /
                    (max_rotations[i].second.w - max_rotations[i].first.w);
            quats[i].x =
                    (quats[i].x - max_rotations[i].first.x) /
                    (max_rotations[i].second.x - max_rotations[i].first.x);
            quats[i].y =
                    (quats[i].y - max_rotations[i].first.y) /
                    (max_rotations[i].second.y - max_rotations[i].first.y);
            quats[i].z =
                    (quats[i].z - max_rotations[i].first.z) /
                    (max_rotations[i].second.z - max_rotations[i].first.z);
            quats[i] = quats[i] / norm(quats[i]);
        }
    }
    return normalized_trajectory;
}
