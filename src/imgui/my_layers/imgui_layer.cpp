#include "imgui_layer.h"
#include <imgui/implot.h>
#include <imgui/imgui_neo_sequencer.h>
#include <fstream>

#define AUTO 0
#define PICK 1

ImGuiLayer::ImGuiLayer(Data *p_data) : data(p_data) {
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    mode = AUTO;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    squat_sampleSize = this->data->motionInfo->size() - 16;
    precomputePathDeviation();
}

void ImGuiLayer::changeInputFile(int p_selected_index) {
    const char *file = this->data->motionInfo->at(p_selected_index + 16).motion_file.c_str();
    data->motionFileProcessor->processInputFile(std::string(file));
    auto algo = data->dtwMap[data->mainLayerContext->dtwVariant];
    auto kNNResults = data->motionFileProcessor->getKClosestMatches(16, std::string(file), algo);
    data->bestMatch = kNNResults.front();
    data->mainLayerContext->reference_file = data->bestMatch->refFile;
    data->mainLayerContext->input_file = data->bestMatch->inputFile;
    data->bestMatch->setSegmentsAndMatchings();
    data->mainLayerContext->mousePos = {0.0f, 0.0f};
    precomputePathDeviation();
}

void ImGuiLayer::UpdateFOVWithScroll() {
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    if (ImGui::IsKeyPressed(ImGuiKey_Space)) {
        PLAY = PAUSE;
    }
    if (io.MouseWheel != 0.0f && io.KeyCtrl) {
        data->refView->fov -= io.MouseWheel * 2;
        if (data->refView->fov < -180.0f)
            data->refView->fov = -180.0f;
        if (data->refView->fov > 180.0f)
            data->refView->fov = 180.0f;
    } else if (io.MouseWheel != 0.0f && io.KeyAlt) {
        data->inputView->fov -= io.MouseWheel * 2;
        if (data->inputView->fov < -180.0f)
            data->inputView->fov = -180.0f;
        if (data->inputView->fov > 180.0f)
            data->inputView->fov = 180.0f;
    }
}

void ImGuiLayer::drawDTWDiagram() {
    ImGuiIO &io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
    auto algo = static_cast<int>(data->mainLayerContext->dtwVariant);
    int n = data->bestMatch->matching_algorithms[algo]->n;
    int m = data->bestMatch->matching_algorithms[algo]->m;

    static int selectedArray = 0;
    ImGui::SameLine();
    ImGui::RadioButton("Path", &selectedArray, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Costs", &selectedArray, 1);

    static ImPlotColormap map = ImPlotColormap_RdBu;
    static float scale_min = s_min;
    static float scale_max = s_max;
    ImGui::SameLine();
    if (ImPlot::ColormapButton(ImPlot::GetColormapName(map), ImVec2(225, 0), map)) {
        map = (map + 1) % ImPlot::GetColormapCount();
        ImPlot::BustColorCache("##Heatmap1");
    }

    float v_speed = 0.1f;
    ImGui::DragFloatRange2("Min / Max", &scale_min, &scale_max, v_speed);

    static ImPlotHeatmapFlags hm_flags = 0;
    ImPlot::PushColormap(map);
    if (ImPlot::BeginPlot("##Heatmap1", ImVec2(-1, -1), ImPlotFlags_NoLegend | ImPlotFlags_NoMouseText)) {
        ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_Opposite | ImPlotAxisFlags_AutoFit,
                          ImPlotAxisFlags_Invert | ImPlotAxisFlags_AutoFit); // No labels and no gridlines
        if (selectedArray == 0) {
            ImPlot::PlotHeatmap("heat", values1.get(), n + 1, m + 1, scale_min, scale_max, nullptr, ImPlotPoint(0, 0),
                                ImPlotPoint(m + 1, n + 1), hm_flags);
        } else {
            ImPlot::PlotHeatmap("heat", values2.get(), n, m, scale_min, scale_max, nullptr, ImPlotPoint(0, 0),
                                ImPlotPoint(m, n), hm_flags);
        }

        // Get mouse position and display it
        ImPlotPoint mousePos = ImPlot::GetPlotMousePos();
        bool isInBounds = mousePos.x >= 0 && mousePos.x < m && mousePos.y >= 0 && mousePos.y < n;
        auto c_x = static_cast<float>(align_path[data->mainLayerContext->c_frame] % (m + 1));
        auto c_y = static_cast<float>(align_path[data->mainLayerContext->c_frame] / (m + 1));
        bool isBoundsAuto = c_x >= 0 && c_x < static_cast<float>(m) && c_y >= 0 && c_y < static_cast<float>(n);

        // Draw cross-hair lines
        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        ImVec2 plot_pos = ImPlot::GetPlotPos();
        ImVec2 plot_size = ImPlot::GetPlotSize();
        ImVec2 mouse_pos = ImPlot::PlotToPixels(mousePos);

        if (ImGui::RadioButton("Auto", &mode, 0)) {
            mode = AUTO;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Select", &mode, 1)) {
            mode = PICK;
        }

        ImGui::SameLine();
        ImGui::Text("Current Frames: (%i, %i)",
                    isInBounds && mode == PICK ? static_cast<int>(mousePos.x) : static_cast<int>(c_x),
                    isInBounds && mode == PICK ? static_cast<int>(mousePos.y) : static_cast<int>(c_y));


        // Draw Selection cross-hair
        if (isInBounds && mode == PICK) {
            data->mainLayerContext->mousePos = (std::pair<int, int>(static_cast<int>(mousePos.x),
                                                                    static_cast<int>(mousePos.y)));
            // Draw vertical line
            draw_list->AddLine(ImVec2(mouse_pos.x, plot_pos.y), ImVec2(mouse_pos.x, plot_pos.y + plot_size.y),
                               IM_COL32(255, 0, 0, 255));
            // Draw horizontal line
            draw_list->AddLine(ImVec2(plot_pos.x, mouse_pos.y), ImVec2(plot_pos.x + plot_size.x, mouse_pos.y),
                               IM_COL32(255, 0, 0, 255));
        }

        ImVec2 c_point = {c_x, c_y};
        ImVec2 align_pos = ImPlot::PlotToPixels(c_point);
        // Draw alignment cross-hair
        if (mode == AUTO && isBoundsAuto) {
            draw_list->AddLine(ImVec2(align_pos.x, plot_pos.y), ImVec2(align_pos.x, plot_pos.y + plot_size.y),
                               IM_COL32(0, 255, 0, 255));
            draw_list->AddLine(ImVec2(plot_pos.x, align_pos.y), ImVec2(plot_pos.x + plot_size.x, align_pos.y),
                               IM_COL32(0, 255, 0, 255));
        }
        data->mainLayerContext->autoMode = (mode == AUTO);
        ImPlot::EndPlot();
    }
}

void ImGuiLayer::showCameraOptions() {
    UpdateFOVWithScroll();
    if (ImGui::CollapsingHeader("Camera Settings", ImGuiTreeNodeFlags_CollapsingHeader)) {
        ImGui::SeparatorText("Reference View (left) Camera##ref");
        //Ref view
        // Aspect Ratio
        ImGui::SliderFloat("Aspect Ratio##ref", &data->refView->aspectRatio, 1.0f, 3.0f);
        // FOV
        ImGui::SliderFloat("Fov##ref", &data->refView->fov, -180.0f, 180.0f);
        // Center
        ImGui::DragFloat3("Center##ref", glm::value_ptr(data->refView->center), 0.1f, -3.0f, 3.0f);
        ImGui::DragFloat3("Position##ref", glm::value_ptr(data->refView->camera_pos), 0.1f, -5.0f, 5.0f);
        ImGui::DragFloat3("Orientation##ref", glm::value_ptr(data->refView->camera_orientation), 0.1f, -1.0f,
                          1.0f);
        // Clear Color
        ImGui::ColorEdit3("Clear Color##ref", (float *) &data->refView->clear_color);
        //Input view
        ImGui::SeparatorText("Input View (right) Camera##input");
        ImGui::SliderFloat("Aspect Ratio##input", &data->inputView->aspectRatio, 1.0f, 3.0f);
        ImGui::SliderFloat("Fov##input", &data->inputView->fov, -180.0f, 180.0f);
        ImGui::DragFloat3("Center##input", glm::value_ptr(data->inputView->center), 0.1f, -3.0f, 3.0f);
        ImGui::DragFloat3("Position##input", glm::value_ptr(data->inputView->camera_pos), 0.1f, -5.0f, 5.0f);
        ImGui::DragFloat3("Orientation##input", glm::value_ptr(data->inputView->camera_orientation), 0.1f, -1.0f,
                          1.0f);
        ImGui::ColorEdit3("Clear Color##input", (float *) &data->inputView->clear_color);
    };
}

void ImGuiLayer::show_selectionTable() {
/*  Loading spinner example
    const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
    const ImU32 bg = ImGui::GetColorU32(ImGuiCol_Button);

    ImGui::Spinner("##spinner", 15, 6, col);
    ImGui::BufferingBar("##buffer_bar", 0.7f, ImVec2(400, 6), bg, col);
    */

    // Options
    static ImGuiTableFlags flags =
            ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable |
            ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti | ImGuiTableFlags_RowBg |
            ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody |
            ImGuiTableFlags_ScrollY;

    if (ImGui::BeginTable("table_sorting", 10, flags, ImVec2(0.0f, 12 * 15), 0.0f)) {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 0.0f, 1);
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, 1);
        ImGui::TableSetupColumn("File", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch, 0.0f,
                                2);
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
                auto pers = data->motionInfo->at(row_n + 16);
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
                ImGui::Text("%d, %d, %d, %d", pers.experience_trainer, pers.expertise, pers.expertise_practical,
                            pers.expertise_theoretical); // Exp
                ImGui::PopID();
            }
        ImGui::EndTable();
    }
}

void ImGuiLayer::show_DTW_Options() {
    if (ImGui::CollapsingHeader("Dynamic Time Warping", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Checkbox("DTW Aligned", &data->mainLayerContext->aligned);
        static int selectedDtw = 0;
        static const char *dtwOptions[] = {"Classic DTW", "Weighted DTW", "Weighted Derivative DTW", "SSC1 DTW",
                                           "SSC2 DTW",
                                           "LW_DTW"};
        ImGui::SameLine();
        ImGui::PushItemWidth(150.0f);
        static float cost = 0.0f;
        if (ImGui::Combo("##dtwVariant", &selectedDtw, dtwOptions, IM_ARRAYSIZE(dtwOptions))) {
            switch (selectedDtw) {
                case 0: // Classic
                {
                    auto kNNResults = data->motionFileProcessor->getKClosestMatches(16, data->bestMatch->inputFile,
                                                                                    DTW);
                    this->data->bestMatch = kNNResults.front();
                    data->bestMatch->setSegmentsAndMatchings();
                    this->data->mainLayerContext->dtwVariant = CLASSIC;
                    data->mainLayerContext->mousePos = {0.0f, 0.0f};
                    cost = data->bestMatch->algorithms_results[DTW];
                    precomputePathDeviation();
                }
                    break;
                case 3: // SSC1_DTW
                {
                    auto kNNResults = data->motionFileProcessor->getKClosestMatches(16, data->bestMatch->inputFile,
                                                                                    SSC1_DTW);
                    this->data->bestMatch = kNNResults.front();
                    data->bestMatch->setSegmentsAndMatchings();
                    this->data->mainLayerContext->dtwVariant = SSC1DTW;
                    data->mainLayerContext->mousePos = {0.0f, 0.0f};
                    cost = data->bestMatch->algorithms_results[SSC1_DTW];
                    precomputePathDeviation();
                }
                    break;
                case 4: // SSC2_DTW
                {
                    auto kNNResults = data->motionFileProcessor->getKClosestMatches(16, data->bestMatch->inputFile,
                                                                                    SSC2_DTW);
                    this->data->bestMatch = kNNResults.front();
                    data->bestMatch->setSegmentsAndMatchings();
                    this->data->mainLayerContext->dtwVariant = SSC2DTW;
                    data->mainLayerContext->mousePos = {0.0f, 0.0f};
                    cost = data->bestMatch->algorithms_results[SSC2DTW];
                    precomputePathDeviation();
                }
                    break;
                case 5: // LW_DTW
                {
                    auto kNNResults = data->motionFileProcessor->getKClosestMatches(16, data->bestMatch->inputFile,
                                                                                    LW_DTW);
                    this->data->bestMatch = kNNResults.front();
                    data->bestMatch->setSegmentsAndMatchings();
                    this->data->mainLayerContext->dtwVariant = LWDTW;
                    data->mainLayerContext->mousePos = {0.0f, 0.0f};
                    cost = data->bestMatch->algorithms_results[LWDTW];
                    precomputePathDeviation();
                }
                    break;
                case 1: // Weighted
                {
                    auto kNNResults = data->motionFileProcessor->getKClosestMatches(16, data->bestMatch->inputFile,
                                                                                    WDTW);
                    this->data->mainLayerContext->dtwVariant = WEIGHTED;
                    data->bestMatch = kNNResults.front();
                    data->bestMatch->setSegmentsAndMatchings();
                    data->mainLayerContext->mousePos = {0.0f, 0.0f};
                    cost = data->bestMatch->algorithms_results[WEIGHTED];
                    precomputePathDeviation();
                }
                    break;
                case 2: // Weighted Derivative
                {
                    auto kNNResults = data->motionFileProcessor->getKClosestMatches(16, data->bestMatch->inputFile,
                                                                                    WDDTW);
                    this->data->mainLayerContext->dtwVariant = WEIGHTED_DERIVATIVE;
                    data->bestMatch = kNNResults.front();
                    data->bestMatch->setSegmentsAndMatchings();
                    data->mainLayerContext->mousePos = {0.0f, 0.0f};
                    cost = data->bestMatch->algorithms_results[WEIGHTED_DERIVATIVE];
                    precomputePathDeviation();
                }
                    break;
            }
        }
        ImGui::PopItemWidth();

        if (data->mainLayerContext->aligned) {
            ImGui::SameLine();
            ImGuiStyle *style = &ImGui::GetStyle();
            style->Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
            int i = 0;
            ImGui::Text(("Cost: " + std::to_string(cost)).c_str());
            ImGui::PopStyleColor();
        }
        ImGui::Checkbox("Show Heatmap", &showDiagram);
        if (showDiagram) {
            data->mainLayerContext->aligned = true;
            drawDTWDiagram();
        }
    }
}

void ImGuiLayer::onRender() {
    ImGuiViewport *viewport = ImGui::GetMainViewport();

    // Set up the main dock space
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
                                    ImGuiWindowFlags_NoBackground;
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
        ImGui::Checkbox("VSync", &data->mainLayerContext
                ->vsync);
        ImGui::SameLine();
        ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
        if (ImGui::CollapsingHeader("Motion Data Selection", ImGuiTreeNodeFlags_DefaultOpen)) {
            show_selectionTable();
        }
        show_DTW_Options();
        ImGui::End();
    }
    ImGui::Begin("##Master Dream", nullptr,
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse |
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings |
                 ImGuiWindowFlags_NoScrollbar);

    auto draw = ImGui::GetBackgroundDrawList();
    draw->AddCircle(ImVec2(1920 / 2, 1080 / 2), 100, 1.0f);

    ImGui::End();
    inputProcessIndicator();
    referenceProcessIndicator();
}

void ImGuiLayer::inputProcessIndicator() {
    ImGui::Begin("Input Progress Indicator");
    int32_t currentFrame;
    if (mode == PICK)
        currentFrame = data->mainLayerContext->mousePos.second;
    else if (data->mainLayerContext->aligned)
        currentFrame = data->c_inp_frame_aligned;
    else
        currentFrame = data->c_inp_frame;
    int32_t startFrame = 0;
    int32_t endFrame = static_cast<int32_t >(data->bestMatch->getInputFrames().size());

    if (ImGui::BeginNeoSequencer("Sequencer1##324", &currentFrame, &startFrame, &endFrame)) {
        // Timeline code here
        ImGui::EndNeoSequencer();
    }
    ImGui::End();
}

void ImGuiLayer::referenceProcessIndicator() {
    ImGui::Begin("Progress Indicators");
    int32_t currentFrame;
    if (mode == PICK)
        currentFrame = data->mainLayerContext->mousePos.first;
    else if (data->mainLayerContext->aligned)
        currentFrame = data->c_ref_frame_aligned;
    else
        currentFrame = data->c_ref_frame;

    int32_t startFrame = 0;
    int32_t endFrame = static_cast<int32_t >(data->bestMatch->getRefFrames().size());

    if (ImGui::BeginNeoSequencer("Sequencer2##34", &currentFrame, &startFrame, &endFrame)) {
        // Timeline code here
        ImGui::EndNeoSequencer();
    }
    ImGui::End();
}
