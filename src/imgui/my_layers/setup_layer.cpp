#include "setup_layer.h"

int SetupLayer::my_image_width;
int SetupLayer::my_image_height;
GLuint SetupLayer::my_image_texture;

int SetupLayer::wdtw_tooltip_width;
int SetupLayer::wdtw_tooltip_height;
GLuint SetupLayer::wdtw_tooltip_texture;

int SetupLayer::wdtw_tooltip_width2;
int SetupLayer::wdtw_tooltip_height2;
GLuint SetupLayer::wdtw_tooltip_texture2;

SetupLayer::SetupLayer(SharedData *data) {
    sharedData = data;
    bool ret = LoadTextureFromFile("../resources/images/hanim2.png", &my_image_texture, &my_image_width,
                                   &my_image_height);
    bool tt = LoadTextureFromFile("../resources/images/tooltips/wdtwtt1.png", &wdtw_tooltip_texture,
                                  &wdtw_tooltip_width,
                                  &wdtw_tooltip_height);
    bool tt2 = LoadTextureFromFile("../resources/images/tooltips/wdtwtt2.png", &wdtw_tooltip_texture2,
                                   &wdtw_tooltip_width2,
                                   &wdtw_tooltip_height2);
    IM_ASSERT(tt);
    IM_ASSERT(tt2);
    IM_ASSERT(ret);
}

void SetupLayer::onRender() {
    ShowAlgorithmSettings();
    ImGui::Begin("Setup");
    ImGui::SameLine();
    // Display the image
    ImGui::Image((void *) (intptr_t) my_image_texture, ImVec2(my_image_width, my_image_height));
    // Get the draw list for custom drawing
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    // Get the position of the upper-left corner of the image
    ImVec2 image_pos = ImGui::GetItemRectMin();

    // Left-shoulder
    addLineCheckBox(draw_list, image_pos, "r_shoulder", 205, 102);
    addLineCheckBox(draw_list, image_pos, "r_sternoclavicular", 240, 100);
    addLineCheckBox(draw_list, image_pos, "vc5", 260, 95);
    addLineCheckBox(draw_list, image_pos, "skullbase", 260, 60);
    addLineCheckBox(draw_list, image_pos, "l_sternoclavicular", 280, 100);

    ImGui::End();
}

void SetupLayer::ShowAlgorithmSettings() {
    const char *distanceOptions[] = {"EUCLID", "MANHATTAN", "CHEBYSHEV"};
    static int distanceMetric = 0; // 0: EUCLID, 1: MANHATTAN, etc.
    static int wdtw_distanceMetric = 0;
    static int wddtw_distanceMetric = 0;
    static int edr_distanceMetric = 0;
    static int twed_distanceMetric = 0;
    static int lcss_distanceMetric = 0;
    static int frechet_distanceMetric = 0;

    AlgoSettings &settings = AlgoSettings::getInstance();

    if (ImGui::Begin("Algorithm Settings")) {
        // DTW
        ImGui::Separator();
        ImGui::Text("DTW Settings");
        ImGui::Combo("##dtw", &distanceMetric, distanceOptions, IM_ARRAYSIZE(distanceOptions));
        settings.dtw_distance = static_cast<Distances>(distanceMetric);
        // WDTW
        ImGui::Separator();
        ImGui::Text("WDTW Settings");
        if (ImGui::BeginItemTooltip()) {
            ImGui::Text("Weighted Dynamic Time Warping (WDTW)");
            ImGui::Image((void *) (intptr_t) wdtw_tooltip_texture, ImVec2(wdtw_tooltip_width, wdtw_tooltip_height));
            ImGui::BulletText("Penalty based DTW");
            ImGui::BulletText("Penalizes points with higher phase difference between reference and input points");
            ImGui::BulletText("Modified logistic weight function assigns weights as function of phase difference");
            ImGui::Separator();
            ImGui::Text("Modified Logistic Weight function(MLWF)");
            ImGui::Image((void *) (intptr_t) wdtw_tooltip_texture2, ImVec2(wdtw_tooltip_width2, wdtw_tooltip_height2));
            ImGui::Text("g: empirical constant that controls the level of penalization for larger phase differences");
            ImGui::Text("w_max: upper bound for weight parameter");
            ImGui::EndTooltip();
        }
        ImGui::Combo("##wdtw", &wdtw_distanceMetric, distanceOptions, IM_ARRAYSIZE(distanceOptions));
        ImGui::SliderFloat("g##wdtw", &settings.wdtw_g, 0.0f, 0.1f);
        ImGui::SliderFloat("w_max##wdtw", &settings.wdtw_w_max, 0.0f, 10.0f);
        settings.wdtw_distance = static_cast<Distances>(wdtw_distanceMetric);

        // WDDTW
        ImGui::Separator();
        ImGui::Text("WDDTW Settings");
        ImGui::Combo("##wddtw", &wddtw_distanceMetric, distanceOptions, IM_ARRAYSIZE(distanceOptions));
        ImGui::SliderFloat("g##wddtw", &settings.wddtw_g, 0.0f, 0.1f);
        ImGui::SliderFloat("w_max##wddtw", &settings.wddtw_w_max, 0.0f, 10.0f);
        settings.wddtw_distance = static_cast<Distances>(wddtw_distanceMetric);

        // EDR
        ImGui::Separator();
        ImGui::Text("EDR Settings");
        ImGui::Combo("##edr", &edr_distanceMetric, distanceOptions, IM_ARRAYSIZE(distanceOptions));
        ImGui::SliderFloat("eps##edr", &settings.edr_epsilon, 0.0f, 1.0f);
        std::cout << "EDR epsilon updated in loop: " << settings.edr_epsilon << std::endl;
        settings.edr_distance = static_cast<Distances>(edr_distanceMetric);

        // TWED
        ImGui::Separator();
        ImGui::Text("TWED Settings");
        ImGui::Combo("##twed", &twed_distanceMetric, distanceOptions, IM_ARRAYSIZE(distanceOptions));
        ImGui::SliderFloat("eps##twed", &settings.twed_nu, 0.0f, 1.0f);
        ImGui::SliderFloat("lam##twed", &settings.twed_lambda, 0.0f, 10.0f);
        settings.twed_distance = static_cast<Distances>(twed_distanceMetric);

        // LCSS
        ImGui::Separator();
        ImGui::Text("LCSS Settings");
        ImGui::Combo("##lcss", &lcss_distanceMetric, distanceOptions, IM_ARRAYSIZE(distanceOptions));
        ImGui::SliderFloat("eps##lcss", &settings.lcss_epsilon, 0.0f, 1.0f);
        ImGui::SliderFloat("del##lcss", &settings.lcss_delta, 0.0f, 10.0f);
        settings.lcss_distance = static_cast<Distances>(lcss_distanceMetric);

        // FRECHET
        ImGui::Separator();
        ImGui::Text("FRECHET Settings");
        ImGui::Combo("##frechet", &frechet_distanceMetric, distanceOptions, IM_ARRAYSIZE(distanceOptions));
        settings.frechet_distance = static_cast<Distances>(frechet_distanceMetric);

        ImGui::Separator();
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4) ImColor::HSV(4 / 7.0f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4) ImColor::HSV(4 / 7.0f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4) ImColor::HSV(4 / 7.0f, 0.8f, 0.8f));
        if (ImGui::Button("Apply Settings")) {
            auto motionFileProcessor = sharedData->currentAnalysis;
            motionFileProcessor->updateTrajectoryManagers();
            auto kNNResults = motionFileProcessor->getKClosestMatches(16, DTW);
            sharedData->trajectoryInfos.clear();
            for (auto result: kNNResults) {
                TrajectoryInfo info;
                info.reference = result->getContext()->reference_file;
                info.manager = result;
                for (int i = 0; i < ALGO_COUNT; i++) {
                    float cost = result->getAlgorithmResult(static_cast<Algorithm>(i));
                    info.costs.push_back(cost);
                }
                sharedData->trajectoryInfos.push_back(info);
            }
            TrajectoryAnalysisManager *manager = kNNResults.front();
            manager->updateDisplayRequirements();
            auto m_Context = DR::getI()->getContext();
            sharedData->inp_segments = calculateSegments(DR::getI()->getInp_frames());
            sharedData->ref_segments = calculateSegments(DR::getI()->getRef_frames());
            sharedData->alignedSegments = m_Context->matching_algorithms[CDTW]->squat_segments;
            sharedData->wdtw_alignedSegments = m_Context->matching_algorithms[WEIGHTDTW]->squat_segments;
        }
        ImGui::PopStyleColor(3);
        ImGui::End();
    }
}

void SetupLayer::addLineCheckBox(ImDrawList *draw_list, ImVec2 &image_pos, const char *label, int s_x, int s_y) {
    ImVec2 checkbox_pos(image_pos.x + s_x, image_pos.y + s_y);
    ImGui::SetCursorScreenPos(checkbox_pos);
    static bool checkbox_value = false;
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255, 255, 255, 255));
    ImGui::Checkbox("##box", &checkbox_value);
    ImGui::PopStyleColor();
}