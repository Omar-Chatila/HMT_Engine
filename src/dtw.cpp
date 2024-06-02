#include <cmath>
#include <algorithm>
#include <iostream>

#include "dtw.h"

float *Dtw::dtw(const Vec3D *v1, const Vec3D *v2, int size_v1, int size_v2, std::function<float(const Vec3D&, const Vec3D&)> func) {
    const int n = size_v1;
    const int m = size_v2;

    float *S = new float[(n + 1) * (m + 1)];
    S[0] = 0;
    for (int i = 1; i <= n; ++i) {
        S[i * (m + 1)] = std::numeric_limits<float>::infinity();
    }
    for (int j = 1; j <= m; ++j) {
        S[j] = std::numeric_limits<float>::infinity();
    }

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            float cost = func(v1[i - 1], v2[j - 1]);
             S[i * (m + 1) + j] = cost + std::min({S[(i-1) * (m + 1) + j], S[i * (m + 1) + (j-1)], S[(i-1) * (m + 1) + (j - 1)]});
        }
    }
    return S;
}

float *Dtw::dtw(const std::vector<Quaternion*> &inp_traj, const std::vector<Quaternion*> &ref_traj, std::function<float(const Quaternion*, const Quaternion*)> func) {
    const int n = inp_traj.size();
    const int m = ref_traj.size();

    float *S = new float[(n + 1) * (m + 1)];
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
            S[i * (m + 1) + j] = cost + std::min({S[(i-1) * (m + 1) + j], S[i * (m + 1) + (j-1)], S[(i-1) * (m + 1) + (j-1)]});
        }
    }

    return S;
}

std::pair<float, std::vector<int>> Dtw::get_cost_and_alignment(float *cost_matrix, int n, int m) {
    // Initialize variables
    int index = (m + 1) * (n + 1) - 1;
    std::vector<int> alignment;
    float cost = cost_matrix[index];

    while (index != 0) {
        alignment.push_back(index); // Record the current index in the alignment path

        // Determine the next index based on the minimum cost neighbor
        int next_index;
        if (cost_matrix[index - (m + 2)] < cost_matrix[index - 1] && cost_matrix[index - (m + 2)] < cost_matrix[index - m - 1]) {
            next_index = index - (m + 2); // Move diagonally up-left
        } else if (cost_matrix[index - 1] < cost_matrix[index - m - 1]) {
            next_index = index - 1; // Move left
        } else {
            next_index = index - m - 1; // Move up
        }

        // Update the cost with the minimum cost of the current step
        cost += cost_matrix[next_index];
        index = next_index; // Move to the next index
    }

    // Reverse the alignment vector to get the correct order
    std::reverse(alignment.begin(), alignment.end());
    // Return the computed total cost and the alignment path
    free(cost_matrix);
    return {cost, alignment};
}