#include <cmath>
#include <algorithm>
#include <fstream>
#include <iostream>
#include "dtw.h"
#include "defines.h"

float *Dtw::dtw(const Vec3D *v1, const Vec3D *v2, int size_v1, int size_v2, std::function<float(const Vec3D&, const Vec3D&)> func) {
    const int n = size_v1;
    const int m = size_v2;

    float *S = (float*) calloc((n + 1) * (m + 1), sizeof(float));
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
             S[current] = cost + std::min({S[above], S[left], S[diag_left]});
        }
    }

    std::ofstream outfile("output.txt");

    for (int i = 0; i <= n; ++i) {
        for (int j = 0; j <= m; ++j) {
            outfile << ", " << S[i * (m + 1) + j];
        }
        outfile << std::endl;
    }

    return S;
}

float *Dtw::dtw(const std::vector<Quaternion*> &inp_traj, const std::vector<Quaternion*> &ref_traj, std::function<float(const Quaternion*, const Quaternion*)> func) {
    const int n = inp_traj.size();
    const int m = ref_traj.size();

    float *S = (float*) calloc((n + 1) * (m + 1), sizeof(float));
    S[0] = 0;
    for (int i = 1; i <= n; ++i) {
        S[i * (m + 1)] = std::numeric_limits<float>::infinity();;
    }
    for (int j = 1; j <= m; ++j) {
        S[j] = std::numeric_limits<float>::infinity();
    }

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            float cost = func(inp_traj[i - 1], ref_traj[j - 1]);
            S[current] = cost + std::min({S[above], S[left], S[diag_left]});
        }
    }

    std::ofstream outfile("output.txt");

    for (int i = 0; i <= n; ++i) {
        for (int j = 0; j <= m; ++j) {
            outfile << ", " << S[i * (m + 1) + j];
        }
        outfile << std::endl;
    }
    return S;
}

std::pair<float, std::vector<int>> Dtw::get_cost_and_alignment(float *cost_matrix, int n, int m) {
    int index = (m + 1) * (n + 1) - 1;
    std::vector<int> alignment;
    const float cost = cost_matrix[index];

    while (index >= m + 2) {
        alignment.push_back(index);
        int next_index;
        if (cost_matrix[index - (m + 2)] < cost_matrix[index - 1] && cost_matrix[index - (m + 2)] < cost_matrix[index - m - 1]) {
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