#include "result_layer.h"
#include "implot.h"

ResultLayer::ResultLayer(Data *p_data) : data(p_data) {
    currentSortIndex = 0;
}

void ResultLayer::sortTrajectoryInfos() {
    if (currentSortIndex == LCSS) {
        std::sort(data->motionFileProcessor->trajectoryInfos.begin(), data->motionFileProcessor->trajectoryInfos.end(),
                  [this](const TrajectoryInfo &a, const TrajectoryInfo &b) {
                      return a.costs[currentSortIndex] > b.costs[currentSortIndex]; // Sort in descending order
                  });
    } else {
        std::sort(data->motionFileProcessor->trajectoryInfos.begin(), data->motionFileProcessor->trajectoryInfos.end(),
                  [this](const TrajectoryInfo &a, const TrajectoryInfo &b) {
                      return a.costs[currentSortIndex] < b.costs[currentSortIndex];
                  });
    }
}

float pearsonCorrelation(const std::vector<float> &x, const std::vector<float> &y) {
    float mean_x = std::accumulate(x.begin(), x.end(), 0.0f) / static_cast<float>(x.size());
    float mean_y = std::accumulate(y.begin(), y.end(), 0.0f) / static_cast<float>(y.size());

    float numerator = 0.0f, denom_x = 0.0f, denom_y = 0.0f;
    for (size_t i = 0; i < x.size(); ++i) {
        numerator += (x[i] - mean_x) * (y[i] - mean_y);
        denom_x += (x[i] - mean_x) * (x[i] - mean_x);
        denom_y += (y[i] - mean_y) * (y[i] - mean_y);
    }
    IM_ASSERT(denom_x != 0.0f);
    IM_ASSERT(denom_y != 0.0f);
    return numerator / std::sqrt(denom_x * denom_y);
}

void calculateCorrelations(const std::vector<std::vector<float>> &costs, Eigen::MatrixXf &correlationMatrix) {
    int algoCount = costs[0].size();
    correlationMatrix.resize(algoCount, algoCount);

    for (int i = 0; i < algoCount; ++i) {
        for (int j = i; j < algoCount; ++j) {
            std::vector<float> x, y;
            for (const auto &cost: costs) {
                x.push_back(cost[i]);
                y.push_back(cost[j]);
            }
            float correlation = pearsonCorrelation(x, y);
            correlationMatrix(i, j) = correlation;
            correlationMatrix(j, i) = correlation;
        }
    }
}

void plotCorrelationMatrix(const Eigen::MatrixXf &correlationMatrix) {
    const char *algorithmNames[] = {"DTW", "WDTW", "WDDTW", "EDR", "TWED", "FRECHET", "FRECHET_QUAT", "LC_FRECHET",
                                    "LCSS"};
    // cheap fix for reversed y-Axis...
    const char *algorithmNames_r[] = {"LCSS", "LC_FRECHET", "FRECHET_QUAT", "FRECHET", "TWED", "EDR", "WDDTW", "WDTW",
                                      "DTW"};

    int numAlgorithms = correlationMatrix.cols();

    if (ImPlot::BeginPlot("Correlation Matrix", ImVec2(-1, 0), ImPlotFlags_NoLegend | ImPlotFlags_NoMouseText)) {
        ImPlot::SetupAxis(ImAxis_X1, "Algorithms", ImPlotAxisFlags_AutoFit);
        ImPlot::SetupAxis(ImAxis_Y1, "Algorithms", ImPlotAxisFlags_AutoFit);

        // Create tick positions
        std::vector<double> positions(numAlgorithms);
        for (int i = 0; i < numAlgorithms; ++i) {
            positions[i] = i;
        }

        // Setup axis ticks with custom labels
        ImPlot::SetupAxisTicks(ImAxis_X1, positions.data(), numAlgorithms, algorithmNames);
        ImPlot::SetupAxisTicks(ImAxis_Y1, positions.data(), numAlgorithms, algorithmNames_r);

        ImPlot::PushColormap(ImPlotColormap_Greys);
        ImPlot::PlotHeatmap("Correlation", correlationMatrix.data(), correlationMatrix.rows(), correlationMatrix.cols(),
                            -1.0f, 1.0f, "%.2f", ImPlotPoint(0, 0), ImPlotPoint(numAlgorithms - 1, numAlgorithms - 1));
        ImPlot::PopColormap();

        ImPlot::EndPlot();
    }
}

void ResultLayer::onRender() {
    ImGui::Begin("Results");
    if (ImGui::BeginTable("TrajectoryCosts", 10, ImGuiTableFlags_Borders)) {
        ImGui::TableSetupColumn("File", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("DTW", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("WDTW", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("WDDTW", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("EDR", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("TWED", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("FRECHET", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("FRECHET_QUAT", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("LC_FRECHET", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("LCSS", ImGuiTableColumnFlags_None);

        // Draw sorting buttons
        ImGui::TableNextColumn();
        ImGui::TableNextColumn();
        for (int i = 0; i < ALGO_COUNT; i++) {
            ImGui::PushID(i);
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4) ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4) ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4) ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
            if (ImGui::Button("Sort")) {
                this->currentSortIndex = i;
                sortTrajectoryInfos();
            }
            ImGui::PopStyleColor(3);
            ImGui::PopID();
            ImGui::TableNextColumn();
        }
        ImGui::TableHeadersRow();

        // Find the top 3 costs for each column (excluding filename and LCSS)
        std::vector<std::vector<float>> top3Costs(ALGO_COUNT);
        for (int i = 0; i < ALGO_COUNT - 1; i++) {
            std::vector<float> costs;
            for (const auto &info: data->motionFileProcessor->trajectoryInfos) {
                costs.push_back(info.costs[i]);
            }
            std::sort(costs.begin(), costs.end());
            for (int j = 0; j < 3 && j < costs.size(); j++) {
                top3Costs[i].push_back(costs[j]);
            }
        }

        // Handle LCSS column separately (find top 3 highest costs)
        {
            std::vector<float> costs;
            for (const auto &info: data->motionFileProcessor->trajectoryInfos) {
                costs.push_back(info.costs[LCSS]);
            }
            std::sort(costs.rbegin(), costs.rend()); // Sort in descending order
            for (int j = 0; j < 3 && j < costs.size(); j++) {
                top3Costs[LCSS].push_back(costs[j]);
            }
        }

        // Render best 3 costs with colors
        int fileIndex = 1;
        for (const auto &info: data->motionFileProcessor->trajectoryInfos) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("#%d: %s", fileIndex++, info.reference.c_str());
            for (int i = 0; i < ALGO_COUNT; i++) {
                ImGui::TableNextColumn();
                float cost = info.costs[i];
                ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // Default color

                if (std::find(top3Costs[i].begin(), top3Costs[i].end(), cost) != top3Costs[i].end()) {
                    // Determine the color based on the rank
                    int rank = std::distance(top3Costs[i].begin(),
                                             std::find(top3Costs[i].begin(), top3Costs[i].end(), cost));
                    switch (rank) {
                        case 0:
                            color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
                            break; // Green for 1st
                        case 1:
                            color = ImVec4(0.5f, 1.0f, 0.0f, 1.0f);
                            break; // Light Green for 2nd
                        case 2:
                            color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
                            break; // Yellow for 3rd
                    }
                }
                ImGui::PushStyleColor(ImGuiCol_Text, color);
                ImGui::Text("%.2f", cost);
                ImGui::PopStyleColor();
            }
        }

        ImGui::EndTable();
    }

    // Plot correlations
    Eigen::MatrixXf correlationMatrix;
    std::vector<std::vector<float>> costs;
    for (const auto &info: data->motionFileProcessor->trajectoryInfos) {
        costs.push_back(info.costs);
    }
    calculateCorrelations(costs, correlationMatrix);
    plotCorrelationMatrix(correlationMatrix);

    ImGui::End();
}
