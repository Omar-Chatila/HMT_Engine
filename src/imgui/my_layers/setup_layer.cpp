#include "setup_layer.h"

SetupLayer::SetupLayer() {
    my_image_width = 0;
    my_image_height = 0;
    my_image_texture = 0;
    bool ret = LoadTextureFromFile("../resources/images/hanim2.png", &my_image_texture, &my_image_width,
                                   &my_image_height);
    IM_ASSERT(ret);
}

void SetupLayer::onRender() {
    ImGui::Begin("Setup");
    ImGui::Dummy(ImVec2(50, 0));
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

void SetupLayer::addLineCheckBox(ImDrawList *draw_list, ImVec2 &image_pos, const char *label, int s_x, int s_y) {
    ImVec2 checkbox_pos(image_pos.x + s_x, image_pos.y + s_y);
    ImGui::SetCursorScreenPos(checkbox_pos);
    static bool checkbox_value = false;
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255, 255, 255, 255));
    ImGui::Checkbox("##box", &checkbox_value);
    ImGui::PopStyleColor();
}