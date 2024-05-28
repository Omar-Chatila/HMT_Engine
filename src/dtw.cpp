#include <cmath>
#include <algorithm>
#include <iostream>

#include "dtw.h"

double *Dtw::dtw(const Vec3D *v1, const Vec3D *v2, int size_v1, int size_v2, double (*func)(Vec3D, Vec3D)) {
    const int n = size_v1 + 1;
    const int m = size_v2 + 1;

    auto *matrix = new double[n * m];
    for (int i = 0; i < m; i++) {
        matrix[i] = 10000;
    }
    for (int i = 0; i < n; i++) {
        matrix[m * i] = 10000;
    }

    matrix[0] = 0;

    for (int i = 1; i <= size_v1; i++) {
        for (int j = 1; j <= size_v2; j++) {
            auto cost = func(v1[i - 1], v2[j - 1]);
            int index = i * (size_v2 + 1) + j;
            matrix[i * m + j] = cost + std::min(matrix[index - m],
                                                std::min(matrix[index - 1],
                                                         matrix[index - m - 1]));
        }
    }
    return matrix;
}

std::pair<double, std::vector<int>> Dtw::get_cost_and_alignment(const double *cost_matrix, int m, int n) {
    // Initialize variables
    int index = (m + 1) * (n + 1) - 1;
    std::vector<int> alignment;
    double cost = cost_matrix[index];

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
    return {cost, alignment};
}





