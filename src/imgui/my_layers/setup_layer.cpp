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

SetupLayer::SetupLayer(Data *p_data) : data(p_data) {
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

void addTooltip(const char *text) {
    if (ImGui::BeginItemTooltip()) {
        ImGui::Text(text);
        ImGui::EndTooltip();
    }
}

void SetupLayer::onRender() {
    ShowAlgorithmSettings();
    AlgoSettings &settings = AlgoSettings::getInstance();
    ImGui::Begin("Setup");
    ImGui::SameLine();
    // Display the image
    ImGui::Image((void *) (intptr_t) my_image_texture, ImVec2(my_image_width, my_image_height));
    // Get the position of the upper-left corner of the image
    ImVec2 image_pos = ImGui::GetItemRectMin();
    ImGui::SetCursorScreenPos({image_pos.x, image_pos.y});
    if (ImGui::Button("All")) std::fill(settings.selected_joints.begin(), settings.selected_joints.end(), true);
    ImGui::SetCursorScreenPos({image_pos.x, image_pos.y + 25});
    if (ImGui::Button("None")) std::fill(settings.selected_joints.begin(), settings.selected_joints.end(), false);
    const int midX = 100;
    addCheckBox(image_pos, "skullbase", midX, 36, settings.selected_joints[Joint::skullbase]);
    addTooltip("Skull base");
    addCheckBox(image_pos, "r_shoulder", 56, 90, settings.selected_joints[Joint::r_shoulder]);
    addTooltip("Right shoulder");
    addCheckBox(image_pos, "r_sternoclavicular", 80, 80, settings.selected_joints[Joint::r_sternoclavicular]);
    addTooltip("Right sternoclavicular joint");
    addCheckBox(image_pos, "vc7", midX, 80, settings.selected_joints[Joint::vc7]);
    addTooltip("Cervical vertebra 7");
    addCheckBox(image_pos, "vt10", midX, 130, settings.selected_joints[Joint::vt10]);
    addTooltip("Thoracic vertebra 10");
    addCheckBox(image_pos, "vl5", midX, 195, settings.selected_joints[Joint::vl5]);
    addTooltip("Lumbar vertebra 5");
    addCheckBox(image_pos, "HumanoidRoot", midX, 220, settings.selected_joints[Joint::HumanoidRoot]);
    addTooltip("Humanoid root");
    addCheckBox(image_pos, "l_hip", midX + 30, 220, settings.selected_joints[Joint::l_hip]);
    addTooltip("Left hip");
    addCheckBox(image_pos, "r_hip", midX - 30, 220, settings.selected_joints[Joint::r_hip]);
    addTooltip("Right hip");
    addCheckBox(image_pos, "l_sternoclavicular", 120, 80, settings.selected_joints[Joint::l_sternoclavicular]);
    addTooltip("Left sternoclavicular joint");
    addCheckBox(image_pos, "l_shoulder", 144, 90, settings.selected_joints[Joint::l_shoulder]);
    addTooltip("Left shoulder");
    addCheckBox(image_pos, "r_elbow", 46, 161, settings.selected_joints[Joint::r_elbow]);
    addTooltip("Right elbow");
    addCheckBox(image_pos, "l_elbow", 154, 161, settings.selected_joints[Joint::l_elbow]);
    addTooltip("Left elbow");
    addCheckBox(image_pos, "r_wrist", 28, 215, settings.selected_joints[Joint::r_wrist]);
    addTooltip("Right wrist");
    addCheckBox(image_pos, "l_wrist", 172, 215, settings.selected_joints[Joint::l_wrist]);
    addTooltip("Left wrist");
    addCheckBox(image_pos, "r_knee", 69, 326, settings.selected_joints[Joint::r_knee]);
    addTooltip("Right knee");
    addCheckBox(image_pos, "l_knee", 131, 326, settings.selected_joints[Joint::l_knee]);
    addTooltip("Left knee");
    addCheckBox(image_pos, "r_ankle", 66, 430, settings.selected_joints[Joint::r_ankle]);
    addTooltip("Right ankle");
    addCheckBox(image_pos, "l_ankle", 134, 430, settings.selected_joints[Joint::l_ankle]);
    addTooltip("Left ankle");
    ImGui::End();
}

void SetupLayer::ShowAlgorithmSettings() {
    const char *distanceOptions[] = {"EUCLID", "MANHATTAN", "CHEBYSHEV"};
    static int distanceMetric = 0;
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
        ImGui::SliderFloat("g##wdtw", &settings.wdtw_g, 0.0f, 0.05f);
        ImGui::SliderFloat("w_max##wdtw", &settings.wdtw_w_max, 0.0f, 10.0f);
        settings.wdtw_distance = static_cast<Distances>(wdtw_distanceMetric);

        // WDDTW
        ImGui::Separator();
        ImGui::Text("WDDTW Settings");
        ImGui::Combo("##wddtw", &wddtw_distanceMetric, distanceOptions, IM_ARRAYSIZE(distanceOptions));
        ImGui::SliderFloat("g##wddtw", &settings.wddtw_g, 0.0f, 0.3f);
        ImGui::SliderFloat("w_max##wddtw", &settings.wddtw_w_max, 0.0f, 10.0f);
        settings.wddtw_distance = static_cast<Distances>(wddtw_distanceMetric);

        // EDR
        ImGui::Separator();
        ImGui::Text("EDR Settings");
        ImGui::Combo("##edr", &edr_distanceMetric, distanceOptions, IM_ARRAYSIZE(distanceOptions));
        ImGui::SliderFloat("eps##edr", &settings.edr_epsilon, 0.0f, 1.0f);
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
            auto motionFileProcessor = data->motionFileProcessor;
            auto currentFile = data->bestMatch->inputFile;
            data->motionFileProcessor->processInputFile(data->bestMatch->inputFile);
            auto knnResults = motionFileProcessor->getKClosestMatches(16, currentFile,
                                                                      data->dtwMap[data->mainLayerContext->dtwVariant]);
            data->bestMatch = knnResults.front();
            data->mainLayerContext->reference_file = data->bestMatch->refFile;
            data->mainLayerContext->input_file = data->bestMatch->inputFile;
            data->bestMatch->setSegmentsAndMatchings();
            data->mainLayerContext->mousePos = {0.0f, 0.0f};
            precomputePathDeviation();
        }
        ImGui::PopStyleColor(3);
        ImGui::End();
    }
}

void SetupLayer::addCheckBox(ImVec2 &image_pos, const char *label, int s_x, int s_y, bool &checkbox_value) {
    ImVec2 checkbox_pos(image_pos.x + s_x - 10, image_pos.y + s_y);
    ImGui::SetCursorScreenPos(checkbox_pos);
    std::string checkbox_label = std::string("##box") + label;
    ImGui::Checkbox(checkbox_label.c_str(), &checkbox_value);
}


