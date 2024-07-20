#include "dtw.h"

#define INF 1000.0f
#define INDEX4(n, m) ((n) * (M + 2) + (m))

float *Dtw::ss1_dtw(const vector<Quaternion *> &inp_traj, const vector<Quaternion *> &ref_traj,
                    std::function<float(const Quaternion *, const Quaternion *,
                                        const std::array<bool, JOINT_COUNT> &)> &func) {
    const size_t N = inp_traj.size();
    const size_t M = ref_traj.size();

    auto *S = (float *) malloc((N + 2) * (M + 2) * sizeof(float));

    for (int n = 0; n < N + 2; ++n) {
        S[INDEX4(n, 0)] = INF;
        S[INDEX4(n, 1)] = INF;
    }
    for (int m = 0; m < M + 2; ++m) {
        S[INDEX4(0, m)] = INF;
        S[INDEX4(1, m)] = INF;
    }

    S[INDEX4(0, 0)] = 0;
    S[INDEX4(1, 1)] = SELECT_F(0, 0);

    for (int n = 2; n < N + 2; ++n) {
        for (int m = 2; m < M + 2; ++m) {
            S[INDEX4(n, m)] = std::min({
                                               S[INDEX4(n - 1, m - 1)],
                                               S[INDEX4(n - 2, m - 1)],
                                               S[INDEX4(n - 1, m - 2)]
                                       }) + SELECT_F(n - 2, m - 2);
        }
    }
    return S;
}

float *Dtw::ss1_dtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                    std::function<float(const Quaternion *, const Quaternion *,
                                        const std::array<float, JOINT_COUNT> &)> &func) {
    const size_t N = inp_traj.size();
    const size_t M = ref_traj.size();

    auto *S = (float *) malloc((N + 2) * (M + 2) * sizeof(float));

    for (int n = 0; n < N + 2; ++n) {
        S[INDEX4(n, 0)] = INF;
        S[INDEX4(n, 1)] = INF;
    }
    for (int m = 0; m < M + 2; ++m) {
        S[INDEX4(0, m)] = INF;
        S[INDEX4(1, m)] = INF;
    }

    S[INDEX4(0, 0)] = 0;
    S[INDEX4(1, 1)] = WEIGHT_F(0, 0);

    for (int n = 2; n < N + 2; ++n) {
        for (int m = 2; m < M + 2; ++m) {
            S[INDEX4(n, m)] = std::min({
                                               S[INDEX4(n - 1, m - 1)],
                                               S[INDEX4(n - 2, m - 1)],
                                               S[INDEX4(n - 1, m - 2)]
                                       }) + WEIGHT_F(n - 2, m - 2);
        }
    }
    return S;
}

std::pair<float, std::vector<int>> Dtw::get_cost_and_alignment_ss1(float *D, int N, int M) {
    int index = (N + 2) * (M + 2) - 1;
    std::vector<int> alignment;
    const float cost = D[index];
    const int ROW = M + 2;
    while (index / (M + 2) >= 2 && index % (M + 2) >= 2) {
        alignment.push_back(index);
        int i1 = index - 1 * (ROW) - 1;
        int i2 = index - 2 * (ROW) - 1;
        int i3 = index - 1 * (ROW) - 2;

        if (D[i1] < D[i2] && D[i1] < D[i3]) {
            index = i1;
        } else if (D[i2] < D[i3]) {
            index = i2;
        } else {
            index = i3;
        }
    }
    std::reverse(alignment.begin(), alignment.end());
    free(D);
    return {cost, alignment};
}

float *Dtw::ssc2_dtw(const vector<Quaternion *> &inp_traj, const vector<Quaternion *> &ref_traj,
                     std::function<float(const Quaternion *, const Quaternion *,
                                         const std::array<float, JOINT_COUNT> &)> &func) {
    const size_t N = inp_traj.size();
    const size_t M = ref_traj.size();
    auto *D = new float[(N + 3) * (M + 3)];

    // Initial values
    for (int n = 0; n < N + 3; ++n) {
        D[INDEX2(n, 0, M)] = INF;
        D[INDEX2(n, 1, M)] = INF;
        D[INDEX2(n, 2, M)] = INF;
    }
    for (int m = 0; m < M + 3; ++m) {
        D[INDEX2(0, m, M)] = INF;
        D[INDEX2(1, m, M)] = INF;
        D[INDEX2(2, m, M)] = INF;
    }

    D[INDEX2(3, 3, M)] = WEIGHT_F(0, 0);

    for (int n = 3; n < N + 3; ++n) {
        for (int m = 3; m < M + 3; ++m) {
            if (n == 3 && m == 3) continue;
            float cost_n_m = WEIGHT_F(n - 3, m - 3);
            float cost_n_1_m = n > 3 ? WEIGHT_F(n - 4, m - 3) : INF;
            float cost_n_m_1 = m > 3 ? WEIGHT_F(n - 3, m - 4) : INF;
            float cost_n_2_m = n > 4 ? WEIGHT_F(n - 5, m - 3) : INF;
            float cost_n_m_2 = m > 4 ? WEIGHT_F(n - 3, m - 5) : INF;
            D[INDEX2(n, m, M)] = std::min({
                                                  D[INDEX2(n - 1, m - 1, M)] + cost_n_m,
                                                  D[INDEX2(n - 2, m - 1, M)] + cost_n_1_m + cost_n_m,
                                                  D[INDEX2(n - 1, m - 2, M)] + cost_n_m_1 + cost_n_m,
                                                  D[INDEX2(n - 3, m - 1, M)] + cost_n_2_m + cost_n_1_m + cost_n_m,
                                                  D[INDEX2(n - 1, m - 3, M)] + cost_n_m_2 + cost_n_m_1 + cost_n_m
                                          });
        }
    }
    //write_matrix_to_file(D, N + 3, M + 3, "SSC2");
    return D;
}

float *Dtw::ssc2_dtw(const vector<Quaternion *> &inp_traj, const vector<Quaternion *> &ref_traj,
                     std::function<float(const Quaternion *, const Quaternion *,
                                         const std::array<bool, JOINT_COUNT> &)> &func) {
    const size_t N = inp_traj.size();
    const size_t M = ref_traj.size();
    auto *D = new float[(N + 3) * (M + 3)];

    // Initial values
    for (int n = 0; n < N + 3; ++n) {
        D[INDEX2(n, 0, M)] = INF;
        D[INDEX2(n, 1, M)] = INF;
        D[INDEX2(n, 2, M)] = INF;
    }
    for (int m = 0; m < M + 3; ++m) {
        D[INDEX2(0, m, M)] = INF;
        D[INDEX2(1, m, M)] = INF;
        D[INDEX2(2, m, M)] = INF;
    }

    D[INDEX2(3, 3, M)] = SELECT_F(0, 0);

    for (int n = 3; n < N + 3; ++n) {
        for (int m = 3; m < M + 3; ++m) {
            if (n == 3 && m == 3) continue;
            float cost_n_m = SELECT_F(n - 3, m - 3);
            float cost_n_1_m = n > 3 ? SELECT_F(n - 4, m - 3) : INF;
            float cost_n_m_1 = m > 3 ? SELECT_F(n - 3, m - 4) : INF;
            float cost_n_2_m = n > 4 ? SELECT_F(n - 5, m - 3) : INF;
            float cost_n_m_2 = m > 4 ? SELECT_F(n - 3, m - 5) : INF;
            D[INDEX2(n, m, M)] = std::min({
                                                  D[INDEX2(n - 1, m - 1, M)] + cost_n_m,
                                                  D[INDEX2(n - 2, m - 1, M)] + cost_n_1_m + cost_n_m,
                                                  D[INDEX2(n - 1, m - 2, M)] + cost_n_m_1 + cost_n_m,
                                                  D[INDEX2(n - 3, m - 1, M)] + cost_n_2_m + cost_n_1_m + cost_n_m,
                                                  D[INDEX2(n - 1, m - 3, M)] + cost_n_m_2 + cost_n_m_1 + cost_n_m
                                          });
        }
    }
    //write_matrix_to_file(D, N + 3, M + 3, "SSC2");
    return D;
}

std::pair<float, std::vector<int>> Dtw::get_cost_and_alignment_ss2(float *D, int N, int M) {
    int index = (N + 3) * (M + 3) - 1;
    std::vector<int> alignment;
    const float cost = D[index];
    const int ROW = M + 3;
    while (index / (M + 3) >= 3 && index % (M + 3) >= 3) {
        alignment.push_back(index);
        int i1 = index - 3 * (ROW) - 1;
        int i2 = index - 2 * (ROW) - 1;
        int i3 = index - 1 * (ROW) - 1;
        int i4 = index - 1 * (ROW) - 2;
        int i5 = index - 1 * (ROW) - 3;
        if (D[i1] < D[i2] && D[i1] < D[i3] && D[i1] < D[i4] && D[i1] < D[i5]) {
            index = i1;
        } else if (D[i2] < D[i3] && D[i2] < D[i4] && D[i2] < D[i5]) {
            index = i2;
        } else if (D[i3] < D[i4] && D[i3] < D[i5]) {
            index = i3;
        } else if (D[i4] < D[i5]) {
            index = i4;
        } else {
            index = i5;
        }
    }
    std::reverse(alignment.begin(), alignment.end());
    free(D);
    return {cost, alignment};
}