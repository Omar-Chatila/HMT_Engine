#include "imgui_layer.h"
#include "motion_file_processor.h"
#include "Engine.h"
#include <thread>
#include <atomic>


ImGuiLayer::ImGuiLayer(UIContext *context) : m_Context(context) {
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
    TrajectoryAnalysisManager* manager = motionFileProcessor->getClosestMatch(DTW);
    manager->updateDisplayRequirements();
    m_Context = DR::getI()->getContext();
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

Color ImGuiLayer::interpolateColor(float value, float minVal, float maxVal, const std::vector<Color> &colorStops) {
    if (colorStops.empty())
        return {0, 0, 0, 255};

    float normalized = (value - minVal) / (maxVal - minVal);
    size_t stopCount = colorStops.size();

    if (normalized <= 0)
        return colorStops.front();
    if (normalized >= 1)
        return colorStops.back();

    float scaled = normalized * (stopCount - 1);
    size_t idx = std::floor(scaled);
    float frac = scaled - idx;

    Color c1 = colorStops[idx];
    Color c2 = colorStops[idx + 1];

    return {
        c1.r + frac * (c2.r - c1.r),
        c1.g + frac * (c2.g - c1.g),
        c1.b + frac * (c2.b - c1.b),
        c1.a + frac * (c2.a - c1.a)};
}

ImU32 ImGuiLayer::interpolateColor(float value, float minVal, float maxVal, ImU32 colorLow, ImU32 colorHigh) {
    float ratio = ((value - minVal) / (maxVal - minVal)) * 1.5f;
    ratio = std::max(0.0f, std::min(1.0f, ratio));

    int r1 = (colorLow >> IM_COL32_R_SHIFT) & 0xFF;
    int g1 = (colorLow >> IM_COL32_G_SHIFT) & 0xFF;
    int b1 = (colorLow >> IM_COL32_B_SHIFT) & 0xFF;
    int a1 = (colorLow >> IM_COL32_A_SHIFT) & 0xFF;

    int r2 = (colorHigh >> IM_COL32_R_SHIFT) & 0xFF;
    int g2 = (colorHigh >> IM_COL32_G_SHIFT) & 0xFF;
    int b2 = (colorHigh >> IM_COL32_B_SHIFT) & 0xFF;
    int a2 = (colorHigh >> IM_COL32_A_SHIFT) & 0xFF;

    int r = static_cast<int>(r1 + ratio * (r2 - r1));
    int g = static_cast<int>(g1 + ratio * (g2 - g1));
    int b = static_cast<int>(b1 + ratio * (b2 - b1));
    int a = static_cast<int>(a1 + ratio * (a2 - a1));

    return IM_COL32(r, g, b, a);
}

void ImGuiLayer::precomputeDeviation(MatrixContext& context, std::vector<float>& distances, std::vector<ImU32>& colors) {
    int n = context.n;
    int m = context.m;
    const std::vector<int>& align_path = context.align_path;
    float* mat = context.mat;

    float minDist = std::numeric_limits<float>::infinity();
    float maxDist = -std::numeric_limits<float>::infinity();

    std::vector<std::pair<int, int>> pathCoords;
    for (int idx : align_path) {
        int i = idx / (m + 1);
        int j = idx % (m + 1);
        pathCoords.emplace_back(i, j);
    }

    distances.resize((n + 1) * (m + 1), std::numeric_limits<float>::infinity());

    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= m; j++) {
            float minDistanceToPath = std::numeric_limits<float>::infinity();
            for (const auto& [pi, pj] : pathCoords) {
                float distance;
                if (context.isCostDeviation) {
                    distance = std::abs(mat[i * (m + 1) + j] - mat[pi * (m + 1) + pj]);
                } else {
                    distance = std::hypot(i - pi, j - pj);
                }
                if (distance < minDistanceToPath) {
                    minDistanceToPath = distance;
                }
            }
            distances[i * (m + 1) + j] = minDistanceToPath;
            if (minDistanceToPath < minDist)
                minDist = minDistanceToPath;
            if (minDistanceToPath > maxDist)
                maxDist = minDistanceToPath;
        }
    }

    colors.resize((n + 1) * (m + 1), IM_COL32(0, 0, 0, 255));

    std::vector<Color> colorStops = {
        {0, 255, 0, 255},   // Green
        {255, 255, 0, 255}, // Yellow
        {255, 0, 0, 255},   // Red
        {127, 0, 255, 255}, // Purple
        {0, 0, 255, 255}    // Blue
    };

    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= m; j++) {
            int index = i * (m + 1) + j;
            bool isAligned = (std::find(align_path.begin(), align_path.end(), index) != align_path.end());
            if (isAligned) {
                colors[index] = IM_COL32_WHITE;
            } else {
                Color interpolatedColor = interpolateColor(
                    (context.isCostDeviation ? distances[index] : distances[index] * 2.0f),
                    minDist, 
                    (context.isCostDeviation ? 0.5 * maxDist : maxDist), 
                    colorStops
                );
                colors[index] = IM_COL32((int)interpolatedColor.r, (int)interpolatedColor.g, (int)interpolatedColor.b, (int)interpolatedColor.a);
            }
        }
    }
}

void ImGuiLayer::precomputeCostDeviation() {
    MatrixContext context{
        std::get<1>(*m_Context->matrix),
        std::get<2>(*m_Context->matrix),
        std::get<3>(*m_Context->matrix),
        m_Context->costmatrix,
        true  // isCostDeviation
    };
    precomputeDeviation(context, distances, colors);
}

void ImGuiLayer::precomputePathDeviation() {
    MatrixContext context{
        std::get<1>(*m_Context->matrix),
        std::get<2>(*m_Context->matrix),
        std::get<3>(*m_Context->matrix),
        std::get<0>(*m_Context->matrix),
        false  // isCostDeviation
    };
    precomputeDeviation(context, distances, colors);
}

void ImGuiLayer::drawDTWDiagram() {
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    const ImVec2 p = ImGui::GetCursorScreenPos();
    int n = std::get<2>(*m_Context->matrix);
    int m = std::get<3>(*m_Context->matrix);
    const float rect_size = 1.0f;
    float xPos = p.x;
    float yPos = p.y;

    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= m; j++) {
            int index = i * (m + 1) + j;
            ImU32 color = colors[index];
            draw_list->AddRectFilled(ImVec2(xPos, yPos), ImVec2(xPos + rect_size, yPos + rect_size), color);
            xPos += rect_size;
        }
        xPos = p.x;
        yPos += rect_size;
    }

    // Advance the ImGui cursor to claim space in the window (otherwise the window will appear small and needs to be resized)
    ImGui::Dummy(ImVec2((m + 1) * rect_size, (n + 1) * rect_size));
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
        // if (ImGui::Button("Reset##CenterRef")) {
        //     m_Context->refView->center = {0.4f, 1.0f, 0.0f};
        // }
        // Position
        ImGui::DragFloat3("Position##ref", glm::value_ptr(m_Context->refView->camera_pos), 0.1f, -5.0f, 5.0f);
        // if (ImGui::Button("Reset##PositionRef")) {
        //     m_Context->refView->camera_pos = {2.0f, 2.0f, 2.0f};
        // }
        ImGui::DragFloat3("Orientation##ref", glm::value_ptr(m_Context->refView->camera_orientation), 0.1f, -1.0f, 1.0f);
        // Clear Color
        ImGui::ColorEdit3("Clear Color##ref", (float *)&m_Context->refView->clear_color);
    //Input view
        ImGui::SeparatorText("Input View (right) Camera##input");
        ImGui::SliderFloat("Aspect Ratio##input", &m_Context->inputView->aspectRatio, 1.0f, 3.0f);
        ImGui::SliderFloat("Fov##input", &m_Context->inputView->fov, -180.0f, 180.0f);
        ImGui::DragFloat3("Center##input", glm::value_ptr(m_Context->inputView->center), 0.1f, -3.0f, 3.0f);
        // if (ImGui::Button("Reset##CenterInp")) {
        //     m_Context->inputView->center = {0.4f, 1.0f, 0.0f};
        // }
        ImGui::DragFloat3("Position##input", glm::value_ptr(m_Context->inputView->camera_pos), 0.1f, -5.0f, 5.0f);
        // if (ImGui::Button("Reset##PositionInp")) {
        //     m_Context->inputView->camera_pos = {2.0f, 2.0f, 2.0f};
        // }
        // Orientation
        ImGui::DragFloat3("Orientation##input", glm::value_ptr(m_Context->inputView->camera_orientation), 0.1f, -1.0f, 1.0f);
        // ImGui::SameLine();
        // if (ImGui::Button("Reset##Orientation")) {
        //     m_Context->refView->camera_orientation = {0.0, 1.0, 0.0};
        //     m_Context->inputView->camera_orientation = {0.0, 1.0, 0.0};
        // }
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
            ImGui::SameLine();
            ImGui::Checkbox("Show Heatmap", &showDiagram);
            if (showDiagram) {
                ImGui::SameLine();
                ImGuiStyle *style = &ImGui::GetStyle();
                char txt_green[] = "text green";
                style->Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                ImGui::Text(("Cost: " + std::to_string(m_Context->cost)).c_str());
                ImGui::PopStyleColor();
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