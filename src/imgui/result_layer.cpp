#include "result_layer.h"

ResultLayer::ResultLayer(SharedData *data) {
    sharedData = data;
}

void ResultLayer::onRender() {
    ImGui::Begin("Results");

    if (ImGui::BeginTable("TrajectoryCosts", 7, ImGuiTableFlags_Borders)) {
        ImGui::TableSetupColumn("File", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("DTW", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("EDR", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("TWED", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("FRECHET", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("LC_FRECHET", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("LCSS", ImGuiTableColumnFlags_None);
        ImGui::TableHeadersRow();

        int fileIndex = 1;
        for (const auto& info : sharedData->trajectoryInfos) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("#%d: %s", fileIndex++, info.reference.c_str());
            for (int i = 0; i < ALGO_COUNT; i++) {
                ImGui::TableNextColumn();
                ImGui::Text("%.2f", info.costs[i]);
            }
        }
        ImGui::EndTable();
    }

    ImGui::End();
}
