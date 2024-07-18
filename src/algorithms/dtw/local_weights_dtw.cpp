#include "dtw.h"

float *Dtw::local_weights_dtw(const vector<Quaternion *> &inp_traj, const vector<Quaternion *> &ref_traj,
                              std::function<float(const Quaternion *, const Quaternion *,
                                                  const std::array<float, JOINT_COUNT> &)> &func,
                              const Vec3D &w) {
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
            S[CURRENT_INDEX] = std::min(
                    {S[ABOVE_INDEX] + w.z * cost, S[LEFT_INDEX] + w.y * cost, S[DIAG_LEFT_INDEX] + w.x * cost});
        }
    }
    //write_matrix_to_file(S, n + 1, m + 1, "dtw_matrix.txt");
    return S;
}

float *Dtw::local_weights_dtw(const vector<Quaternion *> &inp_traj, const vector<Quaternion *> &ref_traj,
                              std::function<float(const Quaternion *, const Quaternion *,
                                                  const std::array<bool, JOINT_COUNT> &)> &func, const Vec3D &w) {
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
            S[CURRENT_INDEX] = std::min(
                    {S[ABOVE_INDEX] + w.z * cost, S[LEFT_INDEX] + w.y * cost, S[DIAG_LEFT_INDEX] + w.x * cost});
        }
    }
    //write_matrix_to_file(S, n + 1, m + 1, "dtw_matrix.txt");
    return S;
}