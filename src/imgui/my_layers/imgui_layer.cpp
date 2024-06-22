#include "imgui_layer.h"
#include <imgui/implot.h>
#include <imgui/implot_internal.h>
#include "../layer_data.h"
#include "motion_file_processor.h"
#include "Engine.h"
#include <fstream>
#include <thread>
#include <atomic>


ImGuiLayer::ImGuiLayer(UIContext *context, SharedData *data) : m_Context(context) {
    sharedData = data;
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    squat_sampleSize = this->m_Context->motion_files->size() - 16;
    precomputePathDeviation();
}

void ImGuiLayer::changeInputFile(int selected_index) {
    const char* file = m_Context->motion_files->at(selected_index + 16).motion_file.c_str();
    MotionFileProcessor* motionFileProcessor = new MotionFileProcessor(SQUATS);
    motionFileProcessor->processInputFile(std::string(file));
    auto kNNResults = motionFileProcessor->getKClosestMatches(16, DTW);
    sharedData->trajectoryInfos.clear();
    for (auto result : kNNResults) {
        TrajectoryInfo info;
        info.reference = result->getContext()->reference_file;
        info.manager = result;
        for (int i = 0; i < ALGO_COUNT; i++) {
            float cost = result->getAlgorithmResult(static_cast<Algorithm>(i));
            info.costs.push_back(cost);
        }
        sharedData->trajectoryInfos.push_back(info);
    }

    TrajectoryAnalysisManager* manager = motionFileProcessor->getClosestMatch(DTW);
    manager->updateDisplayRequirements();
    m_Context = DR::getI()->getContext();
    sharedData->inp_segments = calculateSegments(DR::getI()->getInp_frames());
    sharedData->ref_segments = calculateSegments(DR::getI()->getRef_frames());
    sharedData->alignedSegments = calcSegmentsAligned(std::get<1>(*DR::getI()->getContext()->matrix),DR::getI()->getInp_frames(),DR::getI()->getRef_frames());
    precomputePathDeviation();
}

void ImGuiLayer::UpdateFOVWithScroll() {
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    if (io.MouseWheel != 0.0f && io.KeyCtrl) {
        m_Context->refView->fov -= io.MouseWheel * 2;
        if (m_Context->refView->fov < -180.0f)
            m_Context->refView->fov = -180.0f;
        if (m_Context->refView->fov >  180.0f)
            m_Context->refView->fov = 180.0f;
    } else if (io.MouseWheel != 0.0f && io.KeyAlt) {
        m_Context->inputView->fov -= io.MouseWheel * 2;
        if (m_Context->inputView->fov < -180.0f)
            m_Context->inputView->fov = -180.0f;
        if (m_Context->inputView->fov >  180.0f)
            m_Context->inputView->fov = 180.0f;
    }
}

void ImGuiLayer::precomputeDeviation(MatrixContext& context, std::vector<float>& distances) {
    int n = context.n;
    int m = context.m;
    const std::vector<int>& align_path = context.align_path;
    auto inp_traj = DR::getI()->getInp_frames();
    auto ref_traj = DR::getI()->getRef_frames();
    Trajectories *inT = new Trajectories(inp_traj);
    Trajectories *reT = new Trajectories(ref_traj);
    float* mat = Dtw::get_cost_matrix(inT->get_anglesTrajectories(),
                                    reT->get_anglesTrajectories(),
                                    quaternion_dist);
//    float* mat = context.mat;

    std::vector<std::pair<int, int>> pathCoords;
    for (int idx : align_path) {
        int i = idx / (m + 1);
        int j = idx % (m + 1);
        pathCoords.emplace_back(i, j);
    }

    distances.resize((n + 1) * (m + 1), std::numeric_limits<float>::infinity());
    costM = new float*[n];
    for (int i = 0; i < n; i++) {
        costM[i] = new float[m];
    }


    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= m; j++) {
            float minDistanceToPath = std::numeric_limits<float>::infinity();
            float minCostToPath = std::numeric_limits<float>::infinity();
            int r = 0;
            for (auto num : context.align_path) {
                if (num / (m + 1) == i && num % (m + 1) == j) {
                    r = num;
                    break;
                }
                float costDist = std::abs(mat[i * (m + 1) + j] - mat[num]);
                minCostToPath = std::min(minCostToPath, costDist);
            }

            minCostToPath = std::abs(mat[i * (m + 1) + j] - mat[r]);
            for (const auto& [pi, pj] : pathCoords) {
                float distance;
                distance = std::hypot(i - pi, j - pj);
                if (distance < minDistanceToPath) {
                    minDistanceToPath = distance;
                }
            }
            distances[i * (m + 1) + j] = minDistanceToPath;
            if (i > 0 && j > 0) {
                costM[i - 1][j - 1] = minCostToPath;
            }
        }
    }

    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            if (costM[i][j] != 0)
                costM[i][j] = log10f(costM[i][j]);
    delete inT;
    delete reT;
}

void ImGuiLayer::precomputePathDeviation() {
    MatrixContext context{
            std::get<1>(*m_Context->matrix),
            std::get<2>(*m_Context->matrix),
            std::get<3>(*m_Context->matrix),
            std::get<0>(*m_Context->matrix),
            false  // isCostDeviation
    };
    precomputeDeviation(context, distances);
}

void ImGuiLayer::drawDTWDiagram() {
    int n = std::get<2>(*m_Context->matrix);
    int m = std::get<3>(*m_Context->matrix);
    const float rect_size = 1.0f;
    float values1[n + 1][m + 1];
    float values2[n][m];

    int index = 0;
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= m; j++) {
            values1[i][j] = distances[index++];
        }
    }
    static int selectedArray = 0;

    auto s_min = 10.0f;
    auto s_max = -10.0f;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            values2[i][j] = costM[i][j];
            if (s_max < values2[i][j]) s_max = values2[i][j];
            if (s_min > values2[i][j]) s_min = values2[i][j];
        }
    }
    ImGui::RadioButton("Path", &selectedArray, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Costs", &selectedArray, 1);

    static ImPlotColormap map = ImPlotColormap_Viridis;
    static float scale_min = s_min;
    static float scale_max = s_max;
    ImGui::SameLine();
    if (ImPlot::ColormapButton(ImPlot::GetColormapName(map), ImVec2(225, 0), map)) {
        map = (map + 1) % ImPlot::GetColormapCount();
        ImPlot::BustColorCache("##Heatmap1");
    }

    float v_speed, v_min, v_max;
    if (selectedArray == 0) {
        v_speed = 0.1f;
    }

    ImGui::DragFloatRange2("Min / Max", &scale_min, &scale_max, v_speed);

    static ImPlotHeatmapFlags hm_flags = 0;

    ImPlot::PushColormap(map);

    if (ImPlot::BeginPlot("##Heatmap1", ImVec2(-1, -1), ImPlotFlags_NoLegend | ImPlotFlags_NoMouseText)) {
        ImPlot::SetupAxes(NULL, NULL, 0, 0); // No labels and no gridlines
        if (selectedArray == 0) {
            ImPlot::PlotHeatmap("heat", values1[0], n + 1, m + 1, scale_min, scale_max, NULL, ImPlotPoint(0, 0), ImPlotPoint(m + 1, n + 1), hm_flags);
        } else {
            ImPlot::PlotHeatmap("heat", values2[0], n, m, scale_min, scale_max, NULL, ImPlotPoint(0, 0), ImPlotPoint(m, n), hm_flags);
        }
        ImPlot::EndPlot();
    }

    ImGui::SameLine();
    ImPlot::ColormapScale("##HeatScale", scale_min, scale_max, ImVec2(60, 225));

    float prog = static_cast<float>(m_Context->c_frame % std::get<1>(*m_Context->matrix).size()) / std::get<1>(*m_Context->matrix).size();
    ImGui::ProgressBar(prog, ImVec2((m + 1) * rect_size, 2));
    ImGui::Text("Current Frame: %d", m_Context->c_frame);
}

void ImGuiLayer::showCameraOptions() {
    UpdateFOVWithScroll();
    if (ImGui::CollapsingHeader("Camera Settings", ImGuiTreeNodeFlags_CollapsingHeader)) {
        ImGui::SeparatorText("Reference View (left) Camera##ref");
        //Ref view
        // Aspect Ratio
        ImGui::SliderFloat("Aspect Ratio##ref", &m_Context->refView->aspectRatio, 1.0f, 3.0f);
        // FOV
        ImGui::SliderFloat("Fov##ref", &m_Context->refView->fov, -180.0f, 180.0f);
        // Center
        ImGui::DragFloat3("Center##ref", glm::value_ptr(m_Context->refView->center), 0.1f, -3.0f, 3.0f);
        ImGui::DragFloat3("Position##ref", glm::value_ptr(m_Context->refView->camera_pos), 0.1f, -5.0f, 5.0f);
        ImGui::DragFloat3("Orientation##ref", glm::value_ptr(m_Context->refView->camera_orientation), 0.1f, -1.0f, 1.0f);
        // Clear Color
        ImGui::ColorEdit3("Clear Color##ref", (float *)&m_Context->refView->clear_color);
        //Input view
        ImGui::SeparatorText("Input View (right) Camera##input");
        ImGui::SliderFloat("Aspect Ratio##input", &m_Context->inputView->aspectRatio, 1.0f, 3.0f);
        ImGui::SliderFloat("Fov##input", &m_Context->inputView->fov, -180.0f, 180.0f);
        ImGui::DragFloat3("Center##input", glm::value_ptr(m_Context->inputView->center), 0.1f, -3.0f, 3.0f);
        ImGui::DragFloat3("Position##input", glm::value_ptr(m_Context->inputView->camera_pos), 0.1f, -5.0f, 5.0f);
        ImGui::DragFloat3("Orientation##input", glm::value_ptr(m_Context->inputView->camera_orientation), 0.1f, -1.0f, 1.0f);
        ImGui::ColorEdit3("Clear Color##input", (float *)&m_Context->inputView->clear_color);
    };
}

void ImGuiLayer::show_selectionTable() {
    // Options
    static ImGuiTableFlags flags =
            ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollY;

    if (ImGui::BeginTable("table_sorting", 10, flags, ImVec2(0.0f, 10 * 15), 0.0f)) {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 0.0f, 1);
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, 1);
        ImGui::TableSetupColumn("File", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch, 0.0f, 2);
        ImGui::TableSetupColumn("Sex", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, 3);
        ImGui::TableSetupColumn("Age", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, 3);
        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, 3);
        ImGui::TableSetupColumn("Weight", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, 3);
        ImGui::TableSetupColumn("Hand", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, 3);
        ImGui::TableSetupColumn("Foot", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, 3);
        ImGui::TableSetupColumn("Exp", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, 3);

        ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin(squat_sampleSize - 16);
        while (clipper.Step())
            for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++) {
                auto pers = m_Context->motion_files->at(row_n  + 16);
                // Display a data item
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::PushID(row_n);
                if (ImGui::RadioButton("##active", &selected_index, row_n)) {
                    changeInputFile(selected_index);
                }
                ImGui::TableNextColumn();
                ImGui::Text("%02d", row_n);
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(pers.motion_file.c_str());  // File
                ImGui::TableNextColumn();
                ImGui::Text("%c", pers.sex);  // Sex
                ImGui::TableNextColumn();
                ImGui::Text("%d", pers.age);  // Age
                ImGui::TableNextColumn();
                ImGui::Text("%d", pers.size); // Size
                ImGui::TableNextColumn();
                ImGui::Text("%d", pers.weight); // Weight
                ImGui::TableNextColumn();
                ImGui::Text("%d", pers.right_handed); // Hand
                ImGui::TableNextColumn();
                ImGui::Text("%d", pers.right_footed); // Foot
                ImGui::TableNextColumn();
                ImGui::Text("%d, %d, %d, %d", pers.experience_trainer,pers.expertise, pers.expertise_practical, pers.expertise_theoretical); // Exp
                ImGui::PopID();
            }
        ImGui::EndTable();
    }
}

void ImGuiLayer::show_DTW_Options() {
    if (ImGui::CollapsingHeader("Dynamic Time Warping", ImGuiTreeNodeFlags_DefaultOpen)) {

        ImGui::Checkbox("DTW Aligned", &m_Context->aligned);
        if (m_Context->aligned) {
            ImGui::SameLine();
            ImGuiStyle *style = &ImGui::GetStyle();
            char txt_green[] = "text green";
            style->Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
            ImGui::Text(("Cost: " + std::to_string(m_Context->cost)).c_str());
            ImGui::PopStyleColor();
        }
        ImGui::Checkbox("Show Heatmap", &showDiagram);
        if (showDiagram) {
            m_Context->aligned = true;
            drawDTWDiagram();
        }
    }
}

void ImGuiLayer::onRender() {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    // Set up the main dock space
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse|
                                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));  // Adjust window padding

    ImGui::Begin("DockSpaceWindow", nullptr, window_flags);
    ImGui::PopStyleVar(3);  // Pop all style vars at once

    // Create the dock space
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    ImGui::End();

    {
        static bool show_demo_window = true;
        static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::Begin("Visualizer for Motion Data");

        showCameraOptions();
        ImGui::Checkbox("VSync", &m_Context->vsync);
        if (ImGui::CollapsingHeader("Motion Data Selection", ImGuiTreeNodeFlags_DefaultOpen)) {
            show_selectionTable();
        }
        show_DTW_Options();
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }
}