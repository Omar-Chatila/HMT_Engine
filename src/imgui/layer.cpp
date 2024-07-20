#include "layer.h"

void Layer::precomputePathDeviation() {
    auto data = Data::getInstance();
    auto selected_matching = data->bestMatch->matching_algorithms[data->mainLayerContext->dtwVariant];
    std::cout << "Variant" << static_cast<int>(data->mainLayerContext->dtwVariant) << std::endl;
    IM_ASSERT(selected_matching != nullptr);
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
    values1 = std::make_unique<float[]>((n + 1) * (m + 1));
    values2 = std::make_unique<float[]>(n * m);
    align_path = context.align_path;
    auto *inT = data->bestMatch->inputTrajectories;
    auto *reT = data->bestMatch->refTrajectories;
    float *costMatrix;
    if (AlgoSettings::getInstance().dtw_distance == QUATERNION)
        costMatrix = Dtw::get_cost_matrix(inT->get_anglesTrajectories(),
                                          reT->get_anglesTrajectories(),
                                          quaternion_dist);
    else if (AlgoSettings::getInstance().dtw_distance == QUATERNION_WEIGHTED)
        costMatrix = Dtw::get_cost_matrix(inT->get_anglesTrajectories(),
                                          reT->get_anglesTrajectories(),
                                          quaternion_dist_weighted);
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

    int index = 0;
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
            float distance;
            distance = std::hypot(i - r / (m + 1), j - r % (m + 1));
            if (distance < minDistanceToPath) {
                minDistanceToPath = distance;
            }
            minCostToPath = std::abs(costMatrix[i * (m + 1) + j] - costMatrix[r]);
            p_distances[i * (m + 1) + j] = minDistanceToPath;
            if (i > 0 && j > 0) {
                costM[i - 1][j - 1] = minCostToPath;
            }
            values1[(n - i) * (m + 1) + j] = p_distances[index++];
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (costM[i][j] != 0)
                values2[(n - i - 1) * m + j] = log10f(costM[i][j]);

            if (s_max < values2[i * m + j]) s_max = values2[i * m + j];
            if (s_min > values2[i * m + j]) s_min = values2[i * m + j];
        }
    }

    free(costM);
    free(costMatrix);
}