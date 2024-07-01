#include "setup_layer.h"

SetupLayer::SetupLayer() {
    my_image_width = 0;
    my_image_height = 0;
    my_image_texture = 0;
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
    // WDTW
    static float wdtw_g = 0.005f;
    static float wdtw_w_max = 1.0f;

    static float epsilon = 0.3f;
    static float lambda = 1.0f;
    static float delta = 5.0f;
    static int distanceMetric = 0; // 0: EUCLID, 1: MANHATTAN, etc.
    const char *distanceOptions[] = {"EUCLID", "MANHATTAN", "CHEBYSHEV"};

    if (ImGui::Begin("Algorithm Settings")) {
        // DTW
        ImGui::Separator();
        ImGui::Text("DTW Settings");
        ImGui::SameLine();
        ImGui::Combo("##dtw", &distanceMetric, distanceOptions, IM_ARRAYSIZE(distanceOptions));

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
        ImGui::SliderFloat("g", &wdtw_g, 0.0f, 0.1f);
        ImGui::SliderFloat("w_max", &wdtw_w_max, 0.0f, 10.0f);

        // WDDTW
        ImGui::Separator();
        ImGui::Text("WDDTW Settings");
        ImGui::SliderFloat("g", &wdtw_g, 0.0f, 0.1f);
        ImGui::SliderFloat("w_max", &wdtw_w_max, 0.0f, 10.0f);

        // EDR
        ImGui::Separator();
        ImGui::Text("EDR Settings");
        ImGui::SliderFloat("eps", &epsilon, 0.0f, 1.0f);

        // TWED
        ImGui::Separator();
        ImGui::Text("TWED Settings");
        ImGui::SliderFloat("eps", &epsilon, 0.0f, 1.0f);
        ImGui::SliderFloat("lam", &lambda, 0.0f, 10.0f);

        // LCSS
        ImGui::Separator();
        ImGui::Text("LCSS Settings");
        ImGui::SliderFloat("eps", &epsilon, 0.0f, 1.0f);
        ImGui::SliderFloat("del", &delta, 0.0f, 10.0f);

        // FRECHET
        ImGui::Separator();
        ImGui::Text("FRECHET Settings");
        // (No specific settings)

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