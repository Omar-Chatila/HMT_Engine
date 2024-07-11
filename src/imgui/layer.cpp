//
// Created by omarc on 11/07/2024.
//
#include "layer.h"

void Layer::precomputePathDeviation() {
    auto data = Data::getInstance();
    auto selected_matching = data->bestMatch->matching_algorithms[data->mainLayerContext->dtwVariant];
    MatrixContext context{
            selected_matching->alignmentPath,
            selected_matching->n,
            selected_matching->m,
            selected_matching->distMatrix,
            false
    };
    precomputeDeviation(context, distances);
}

void Layer::precomputeDeviation(MatrixContext &context, std::vector<float> &p_distances) {
    auto data = Data::getInstance();
    align_path.clear();
    values1.reset();
    values2.reset();
    pathCoords.clear();
    int n = context.n;
    int m = context.m;
    std::cout << data->bestMatch->matching_algorithms[WEIGHTDTW]->alignmentPath.size() << std::endl;
    values1 = std::make_unique<float[]>((n + 1) * (m + 1));
    values2 = std::make_unique<float[]>(n * m);
    align_path = context.align_path;
    auto inp_traj = data->bestMatch->getInputFrames();
    auto ref_traj = data->bestMatch->getRefFrames();
    auto *inT = data->bestMatch->inputTrajectories;
    auto *reT = data->bestMatch->refTrajectories;
    // TODO: cost matrix anhand der DTW variante berechnen
    float *costMatrix = Dtw::get_cost_matrix(inT->get_anglesTrajectories(),
                                             reT->get_anglesTrajectories(),
                                             quaternion_dist);
    for (int idx: align_path) {
        int i = idx / (m + 1);
        int j = idx % (m + 1);
        pathCoords.emplace_back(i, j);
    }

    p_distances.resize((n + 1) * (m + 1), std::numeric_limits<float>::infinity());
    costM = new float *[n];
    for (int i = 0; i < n; i++) {
        costM[i] = new float[m];
    }

    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= m; j++) {
            float minDistanceToPath = std::numeric_limits<float>::infinity();
            float minCostToPath;
            int r = 0;
            for (auto num: context.align_path) {
                if (num / (m + 1) == i && num % (m + 1) == j) {
                    r = num;
                    break;
                }
            }
            minCostToPath = std::abs(costMatrix[i * (m + 1) + j] - costMatrix[r]);
            for (const auto &[pi, pj]: pathCoords) {
                float distance;
                distance = std::hypot(i - pi, j - pj);
                if (distance < minDistanceToPath) {
                    minDistanceToPath = distance;
                }
            }
            p_distances[i * (m + 1) + j] = minDistanceToPath;
            if (i > 0 && j > 0) {
                costM[i - 1][j - 1] = minCostToPath;
            }
        }
    }

    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            if (costM[i][j] != 0)
                values2[(n - i - 1) * m + j] = log10f(costM[i][j]);

    int index = 0;
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= m; j++) {
            values1[(n - i) * (m + 1) + j] = p_distances[index++];
        }
    }
    s_min = 10.0f;
    s_max = -10.0f;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (s_max < values2[i * m + j]) s_max = values2[i * m + j];
            if (s_min > values2[i * m + j]) s_min = values2[i * m + j];
        }
    }
    free(costM);
    free(costMatrix);
}


