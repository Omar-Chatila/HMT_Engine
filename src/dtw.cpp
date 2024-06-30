//
// Created by omarc on 11/04/2024.
//

#include "../include/dtw.h"
#include "../include/distance_measures.h"
#include <cmath>
#include <algorithm>
#include <iostream>

double *Dtw::dtw(const double *v1, const double *v2, int size_v1, int size_v2, double (*func)(double, double)) {
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

double *Dtw::dtw(const Point3D *v1, const Point3D *v2, int size_v1, int size_v2, double (*func)(Point3D, Point3D)) {
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

vector<vector<double>> Dtw::get_curves_from_csv() {
    vector<vector<double>> curves;
    std::ifstream movement_data("../bvh_parser/walk1.csv");
    std::string myline;
    int index = 0;
    if (movement_data.is_open()) {
        while (movement_data) {
            std::getline(movement_data, myline);
            std::stringstream ss(myline);
            std::string token;
            std::vector<std::string> line;

            while (std::getline(ss, token, ',')) {
                line.push_back(token);
                // No rotation data for now
            }

            if (!line[0].contains("rot") && !line[0].contains("Frames")) {
                index++;
                vector<double> curve_points;
                for (int i = 1; i < line.size(); i++) {
                    double num = std::stod(line[i]);
                    curve_points.push_back(num);
                }
                curves.push_back(curve_points);
            }
        }
    } else {
        std::cerr << "Error opening file\n";
    }
    return curves;
}

pair<double, vector<int>> Dtw::run_example(double *vals1, double *vals2, int n, int m, double (*func)(double, double)) {
    auto *matrix = Dtw::dtw(vals1, vals2, n, m, func);
    for (int i = 0; i < (n + 1) * (m + 1); i++) {
        auto num = matrix[i];
        // if (i % (m + 1) != 0 && (i / (m + 1)) != 0)
        std::cout << num << ", ";
        if (i % (m + 1) == m) {
            std::cout << '\n';
        }
    }
    auto pair = Dtw::get_cost_and_alignment(matrix, m, n);
    std::cout << "Alignment: " << std::endl;
    for (auto num : pair.second) {
        std::cout << num << " ";
    }
    std::cout << "\nCost: " << pair.first << std::endl;
    delete[] matrix;
    return pair;
}

void Dtw::set_frames(double *vals1, double *vals2, int m, int n, vector<double *> &frames1, vector<double *> &frames2) {
    srand(time(NULL));
    for (int i = 0; i < 360; i++) {
        double *curve1 = (double *)(malloc(n * sizeof(double)));
        double *curve2 = (double *)(malloc(m * sizeof(double)));

        for (int i = 0; i < n; i++) {
            int deviation = rand() % 100;
            int v = vals1[i] + static_cast<double>(deviation / 50.0);
            curve1[i] = vals1[i] + static_cast<double>(deviation / 50.0);
        }

        for (int i = 0; i < m; i++) {
            int deviation = rand() % 100;
            int v = vals2[i] + static_cast<double>(deviation / 50.0);
            curve2[i] = vals2[i] + static_cast<double>(deviation / 50.0);
        }

        frames1.push_back(curve1);
        frames2.push_back(curve2);
    }
}




