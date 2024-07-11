#include "result_layer.h"
#include <algorithm> // For std::min_element and std::max_element

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
    ImGui::End();
}
