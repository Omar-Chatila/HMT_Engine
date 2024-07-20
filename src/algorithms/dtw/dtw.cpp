#include <cmath>
#include <algorithm>
#include "dtw.h"
#include "trajectories.h"

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
                std::function<float(const Quaternion *, const Quaternion *,
                                    const std::array<bool, JOINT_COUNT> &selectedJ)> &func) {
    const int n = inp_traj.size();
    const int m = ref_traj.size();

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
            float cost = func(inp_traj[i - 1], ref_traj[j - 1], AlgoSettings::getInstance().selected_joints);
            S[CURRENT_INDEX] = cost + std::min({S[ABOVE_INDEX], S[LEFT_INDEX], S[DIAG_LEFT_INDEX]});
        }
    }
    //write_matrix_to_file(S, n + 1, m + 1, "dtw_matrix.txt");
    return S;
}

float *Dtw::dtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                std::function<float(const Quaternion *, const Quaternion *,
                                    const std::array<float, JOINT_COUNT> &selectedJ)> &func) {
    const int n = inp_traj.size();
    const int m = ref_traj.size();

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
            float cost = func(inp_traj[i - 1], ref_traj[j - 1], AlgoSettings::getInstance().joint_weights);
            S[CURRENT_INDEX] = cost + std::min({S[ABOVE_INDEX], S[LEFT_INDEX], S[DIAG_LEFT_INDEX]});
        }
    }
    //write_matrix_to_file(S, n + 1, m + 1, "dtw_matrix.txt");
    return S;
}

float *Dtw::standardized_dtw(const vector<Quaternion *> &inp_traj, const vector<Quaternion *> &ref_traj,
                             std::function<float(const Quaternion *, const Quaternion *,
                                                 const std::array<bool, JOINT_COUNT> &)> &func) {
    return dtw(inp_traj, ref_traj, func);
}

float *Dtw::standardized_dtw(const vector<Quaternion *> &inp_traj, const vector<Quaternion *> &ref_traj,
                             std::function<float(const Quaternion *, const Quaternion *,
                                                 const std::array<float, JOINT_COUNT> &)> &func) {
    return dtw(inp_traj, ref_traj, func);
}

std::pair<float, std::vector<int>> Dtw::get_cost_and_alignment(float *cost_matrix, int n, int m) {
    int index = INDEX(n, m);
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
    free(cost_matrix);
    return {cost, alignment};
}

float *Dtw::get_cost_matrix(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                            std::function<float(const Quaternion *, const Quaternion *,
                                                const std::array<bool, JOINT_COUNT> &selectedJ)> &func) {
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
            S[CURRENT_INDEX] = func(inp_traj[i - 1], ref_traj[j - 1], AlgoSettings::getInstance().selected_joints);
        }
    }
    return S;
}

float *Dtw::get_cost_matrix(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                            std::function<float(const Quaternion *, const Quaternion *,
                                                const std::array<float, JOINT_COUNT> &selectedJ)> &func) {
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
            S[CURRENT_INDEX] = func(inp_traj[i - 1], ref_traj[j - 1], AlgoSettings::getInstance().joint_weights);
        }
    }
    return S;
}

std::vector<Quaternion *> Dtw::deep_copy(const vector<Quaternion *> &original) {
    std::vector<Quaternion *> copy;
    copy.reserve(original.size());
    for (auto *q: original) {
        auto *joints = (Quaternion *) (malloc(JOINT_COUNT * sizeof(Quaternion)));
        for (int i = 0; i < JOINT_COUNT; i++) {
            joints[i] = new Quaternion(q[i]);
        }
        copy.push_back(joints);
    }
    return copy;
}

void Dtw::write_matrix_to_file(float *S, int rows, int cols, const std::string &filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                file << S[i * cols + j] << " ";
            }
            file << std::endl;
        }
        file.close();
    } else {
        std::cerr << "Unable to open file " << filename << std::endl;
    }
}