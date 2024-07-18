//
// Created by omarc on 16/07/2024.
//
#include "dtw.h"

float *Dtw::ss1_dtw(const vector<Quaternion *> &inp_traj, const vector<Quaternion *> &ref_traj,
                    std::function<float(const Quaternion *, const Quaternion *,
                                        const std::array<bool, JOINT_COUNT> &)> &func) {
    const int n = inp_traj.size();
    const int m = ref_traj.size();

    auto *S = (float *) (malloc((n + 1) * (m + 1) * sizeof(float)));
    S[0] = 0;
    S[INDEX(1, 1)] = SELECT_F(1, 1);
    for (int i = 1; i <= n; ++i) {
        S[INDEX(i, 0)] = std::numeric_limits<float>::infinity();
        if (i > 1)
            S[INDEX(i, 1)] = std::numeric_limits<float>::infinity();
    }
    for (int j = 1; j <= m; ++j) {
        S[INDEX(0, j)] = std::numeric_limits<float>::infinity();
        if (j > 1)
            S[INDEX(1, j)] = std::numeric_limits<float>::infinity();

    }

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            float cost = SELECT_F(i - 1, j - 1);
            S[INDEX(i, j)] = cost + std::min({S[INDEX(i - 1, j - 1)], S[INDEX(i - 1, j - 1)], S[INDEX(i - 1, j - 2)]});
        }
    }
    //write_matrix_to_file(S, n + 1, m + 1, "dtw_matrix.txt");
    return S;
}

float *Dtw::ss1_dtw(const std::vector<Quaternion *> &inp_traj, const std::vector<Quaternion *> &ref_traj,
                    std::function<float(const Quaternion *, const Quaternion *,
                                        const std::array<float, JOINT_COUNT> &)> &func) {
    const int n = inp_traj.size();
    const int m = ref_traj.size();

    auto *S = (float *) (malloc((n + 1) * (m + 1) * sizeof(float)));
    S[0] = 0;
    S[INDEX(1, 1)] = WEIGHT_F(1, 1);
    for (int i = 1; i <= n; ++i) {
        S[INDEX(i, 0)] = std::numeric_limits<float>::infinity();
        if (i > 1)
            S[INDEX(i, 1)] = std::numeric_limits<float>::infinity();
    }
    for (int j = 1; j <= m; ++j) {
        S[INDEX(0, j)] = std::numeric_limits<float>::infinity();
        if (j > 1)
            S[INDEX(1, j)] = std::numeric_limits<float>::infinity();

    }

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            float cost = WEIGHT_F(i - 1, j - 1);
            S[INDEX(i, j)] = cost + std::min({S[INDEX(i - 1, j - 1)], S[INDEX(i - 1, j - 1)], S[INDEX(i - 1, j - 2)]});
        }
    }
    //write_matrix_to_file(S, n + 1, m + 1, "dtw_matrix.txt");
    return S;
}

std::pair<float, std::vector<int>> Dtw::get_cost_and_alignment_ss1(float *cost_matrix, int n, int m) {
    int index = INDEX(n, m);
    std::vector<int> alignment;
    const float cost = cost_matrix[index];

    while (index > 0) {
        alignment.push_back(index);
        int next_index;
        if (cost_matrix[index - 1 - (m + 1)] < cost_matrix[index - 2 - (m + 1)] &&
            cost_matrix[index - (m + 2)] < cost_matrix[index - 1 - 2 * (m + 1)]) {
            next_index = index - 1 - (m + 1);
        } else if (cost_matrix[index - 2 - (m + 1)] < cost_matrix[index - 2 * (m + 1) - 1]) {
            next_index = index - 2 - (m + 1);
        } else {
            next_index = index - 2 * (m + 1) - 1;
        }
        index = next_index; // Move to the next index
    }
    std::reverse(alignment.begin(), alignment.end());
    free(cost_matrix);
    return {cost, alignment};
}

float *Dtw::ssc2_dtw(const vector<Quaternion *> &inp_traj, const vector<Quaternion *> &ref_traj,
                     std::function<float(const Quaternion *, const Quaternion *,
                                         const std::array<float, JOINT_COUNT> &)> &func) {
    const int N = inp_traj.size();
    const int M = ref_traj.size();
    float *D = new float[(N + 3) * (M + 3)];

    // Initialize the array with infinity
    for (int i = 0; i < (N + 3) * (M + 3); ++i) {
        D[i] = std::numeric_limits<float>::infinity();
    }

    // Initial values
    for (int n = -2; n <= N; ++n) {
        D[INDEX2(n + 2, -2 + 2, M)] = std::numeric_limits<float>::infinity();
        D[INDEX2(n + 2, -1 + 2, M)] = std::numeric_limits<float>::infinity();
        D[INDEX2(n + 2, 0 + 2, M)] = std::numeric_limits<float>::infinity();
    }
    for (int m = -2; m <= M; ++m) {
        D[INDEX2(-2 + 2, m + 2, M)] = std::numeric_limits<float>::infinity();
        D[INDEX2(-1 + 2, m + 2, M)] = std::numeric_limits<float>::infinity();
        D[INDEX2(0 + 2, m + 2, M)] = std::numeric_limits<float>::infinity();
    }

    D[INDEX2(3, 3, M)] = WEIGHT_F(0, 0);

    for (int n = 1; n <= N; ++n) {
        for (int m = 1; m <= M; ++m) {
            float cost = WEIGHT_F(n - 1, m - 1);
            D[INDEX2(n + 2, m + 2, M)] = std::min({
                                                          D[INDEX2(n - 1 + 2, m - 1 + 2, M)] + cost,
                                                          D[INDEX2(n - 2 + 2, m - 1 + 2, M)] + WEIGHT_F(n - 2, m - 1) +
                                                          cost,
                                                          D[INDEX2(n - 1 + 2, m - 2 + 2, M)] + WEIGHT_F(n - 1, m - 2) +
                                                          cost,
                                                          D[INDEX2(n - 3 + 2, m - 1 + 2, M)] + WEIGHT_F(n - 3, m - 1) +
                                                          WEIGHT_F(n - 2, m - 1) + cost,
                                                          D[INDEX2(n - 1 + 2, m - 3 + 2, M)] + WEIGHT_F(n - 1, m - 3) +
                                                          WEIGHT_F(n - 1, m - 2) + cost
                                                  });
        }
    }
    return D;
}

float *Dtw::ssc2_dtw(const vector<Quaternion *> &inp_traj, const vector<Quaternion *> &ref_traj,
                     std::function<float(const Quaternion *, const Quaternion *,
                                         const std::array<bool, JOINT_COUNT> &)> &func) {
    const int N = inp_traj.size();
    const int M = ref_traj.size();
    float *D = new float[(N + 3) * (M + 3)];

    // Initialize the array with infinity
    for (int i = 0; i < (N + 3) * (M + 3); ++i) {
        D[i] = std::numeric_limits<float>::infinity();
    }

    // Initial values
    for (int n = -2; n <= N; ++n) {
        D[INDEX2(n + 2, -2 + 2, M)] = std::numeric_limits<float>::infinity();
        D[INDEX2(n + 2, -1 + 2, M)] = std::numeric_limits<float>::infinity();
        D[INDEX2(n + 2, 0 + 2, M)] = std::numeric_limits<float>::infinity();
    }
    for (int m = -2; m <= M; ++m) {
        D[INDEX2(-2 + 2, m + 2, M)] = std::numeric_limits<float>::infinity();
        D[INDEX2(-1 + 2, m + 2, M)] = std::numeric_limits<float>::infinity();
        D[INDEX2(0 + 2, m + 2, M)] = std::numeric_limits<float>::infinity();
    }
    D[INDEX2(3, 3, M)] = SELECT_F(0, 0);
    for (int n = 1; n <= N - 2; ++n) {
        for (int m = 1; m <= M - 2; ++m) {
            float cost = SELECT_F(n - 1 + 2, m - 1 + 2);
            //std::cout << n << "," << m << std::endl;
            D[INDEX2(n + 2, m + 2, M)] = std::min({
                                                          D[INDEX2(n - 1 + 2, m - 1 + 2, M)] + cost,
                                                          D[INDEX2(n - 2 + 2, m - 1 + 2, M)] +
                                                          SELECT_F(n - 2 + 2, m - 1 + 2) +
                                                          cost,
                                                          D[INDEX2(n - 1 + 2, m - 2 + 2, M)] +
                                                          SELECT_F(n - 1 + 2, m - 2 + 2) +
                                                          cost,
                                                          D[INDEX2(n - 3 + 2, m - 1 + 2, M)] +
                                                          SELECT_F(n - 3 + 2, m - 1 + 2) +
                                                          SELECT_F(n - 2 + 2, m - 1 + 2) + cost,
                                                          D[INDEX2(n - 1 + 2, m - 3 + 2, M)] +
                                                          SELECT_F(n - 1 + 2, m - 3 + 2) +
                                                          SELECT_F(n - 1 + 2, m - 2 + 2) + cost
                                                  });
        }
    }
    return D;
}

std::pair<float, std::vector<int>> Dtw::get_cost_and_alignment_ss2(float *D, int N, int M) {
    int index = (N + 3) * (M + 3) - 1;
    std::vector<int> alignment;
    const float cost = D[index];
    const int ROW = M + 3;
    while (index > 0) {
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