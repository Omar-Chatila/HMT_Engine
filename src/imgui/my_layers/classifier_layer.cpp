#include "classifier_layer.h"

ClassifierLayer::ClassifierLayer(SharedData *data) {
    sharedData = data;
    this->currentFrame = &DR::getI()->getContext()->c_frame;
    this->is_continuous = true;
    ImGui::CreateContext();
    ImPlot::CreateContext();
}

ClassifierLayer::~ClassifierLayer() {
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}

void ClassifierLayer::onRender() {
    ImGui::Begin("Errors");
    ImGui::BulletText("Occurrence of each error pattern");
    ImGui::SameLine();
    ImGui::Checkbox("Continuous##", &this->is_continuous);
    this->currentFrame = &DR::getI()->getContext()->c_frame;
    std::vector<Frame> inp_frames = DR::getI()->getInp_frames();
    int num_frames = inp_frames.size();
    int c_t = *this->currentFrame % num_frames;

    static std::vector<float> x_data;
    static std::vector<std::vector<float>> y_data(ERROR_COUNT + 2, std::vector<float>(num_frames));

    x_data.resize(num_frames);
    for (int frame_idx = 0; frame_idx < num_frames; ++frame_idx) {
        x_data[frame_idx] = static_cast<float>(frame_idx);
        std::array<Vec3D, ERROR_COUNT> errors = inp_frames[frame_idx].meta_info.parameters;
        for (int i = 0; i < ERROR_COUNT; ++i) {
            y_data[i][frame_idx] = errors[i].x * errors[i].y * errors[i].z;
        }
        y_data[ERROR_COUNT][frame_idx] = -0.05;
        y_data[ERROR_COUNT + 1][frame_idx] = 1.05;
    }

    // Determine the number of frames to plot
    int plot_frames = c_t + 1; // this->is_continuous ? c_t + 1 : num_frames;
    if (ImPlot::BeginPlot("Error Plot", ImVec2(-1, 250))) {
        ImPlot::SetupAxes("Time [ms]", "Values", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, num_frames - 1, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, -0.1f, 1.1f);
        ImPlot::SetupLegend(ImPlotLocation_East, ImPlotLegendFlags_Outside);

        for (int i = 0; i < ERROR_COUNT + 2; ++i) {
            std::string label = "";
            if (i < ERROR_COUNT)
                label = errorPatternMap.at(static_cast<ErrorPattern>(i));
            ImPlot::PlotLine(label.c_str(), x_data.data(), y_data[i].data(), plot_frames); // Plot up to the determined number of frames
        }

        ImPlot::EndPlot();
    }
    ImGui::End();
}
