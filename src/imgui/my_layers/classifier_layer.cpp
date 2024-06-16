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
    errorPlot();
    segmentPlot();
    segmentPlotUnAligned();
    ImGui::End();
}

void ClassifierLayer::errorPlot() {
    ImGui::Checkbox("Continuous##", &this->is_continuous);
    this->currentFrame = &DR::getI()->getContext()->c_frame;
    std::vector<Frame> inp_frames = DR::getI()->getInp_frames();
    int num_frames = std::get<1>(*DR::getI()->getContext()->matrix).size();
    int c_t = *this->currentFrame;
    static std::vector<float> x_data;
    static std::vector<std::vector<float>> y_data(ERROR_COUNT + 2, std::vector<float>());

    x_data.resize(num_frames);
    for (auto &vec : y_data) {
        vec.resize(num_frames, 0.0f);
    }

    int m = DR::getI()->getRef_frames().size();

    for (int frame_idx = 0; frame_idx < num_frames; ++frame_idx) {
        int align_index = std::get<1>(*DR::getI()->getContext()->matrix)[frame_idx];
        int inp_index = align_index / (m + 1) - 1;
        x_data[frame_idx] = static_cast<float>(frame_idx);
        std::array<Vec3D, ERROR_COUNT> errors = inp_frames[inp_index].meta_info.parameters;
        for (int i = 0; i < ERROR_COUNT; ++i) {
            float value = errors[i].x * errors[i].y * errors[i].z;
            y_data[i][frame_idx] = value;
        }
        y_data[ERROR_COUNT][frame_idx] = -0.05;
        y_data[ERROR_COUNT + 1][frame_idx] = 1.05;
    }

    int plot_frames = this->is_continuous ? c_t + 1 : num_frames;
    if (ImPlot::BeginPlot("Occurrence of each error pattern", ImVec2(-1, 200))) {
        ImPlot::SetupAxes("Time [ms]", "Values", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, num_frames - 1, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, -0.1f, 1.1f);
        ImPlot::SetupLegend(ImPlotLocation_East, ImPlotLegendFlags_Outside);

        for (int i = 0; i < ERROR_COUNT + 2; ++i) {
            std::string label = "";
            if (i < ERROR_COUNT)
                label = errorPatternMap.at(static_cast<ErrorPattern>(i));
            ImPlot::PlotLine(label.c_str(), x_data.data(), y_data[i].data(), plot_frames);
        }
        ImPlot::EndPlot();
    }
}

void SetupPlot(const char* title, const char* const* politicians, int* data_reg, const char* const* labels_reg, ImPlotColormap colormap) {
    ImPlot::PushColormap(colormap);
    if (ImPlot::BeginPlot(title, ImVec2(1670, 200), ImPlotFlags_NoMouseText)) {
        ImPlot::SetupLegend(ImPlotLocation_South, ImPlotLegendFlags_Outside | ImPlotLegendFlags_Horizontal);
        ImPlot::SetupAxes(NULL, NULL, ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_Invert);
        ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(10, 10));
        ImPlot::SetupAxisTicks(ImAxis_Y1, 0, 1, 2, politicians, false);
        ImPlot::PlotBarGroups(labels_reg, data_reg, 5, 2, 0.75, 0, ImPlotBarGroupsFlags_Stacked | ImPlotBarGroupsFlags_Horizontal);
        ImPlot::PopStyleVar();
        ImPlot::EndPlot();
    }
    ImPlot::PopColormap();
}

void PrepareData(const std::array<int, 10>& values, int* data_reg) {
    for (int i = 0; i < 10; i++) {
        data_reg[i] = std::max(values[i], 0);
    }
    for (int i = 2; i < 10; i++) {
        data_reg[i] -= values[i - 2];
    }
}

void PrepareUnalignedData(const std::array<int, 5>& in_values, const std::array<int, 5>& ref_values, int* data_reg2) {
    for (int i = 0; i < 10; i++) {
        if (i % 2 == 0)
            data_reg2[i] = std::max(in_values[i / 2], 0);
        else
            data_reg2[i] = std::max(ref_values[i / 2], 0);
    }
    for (int i = 2; i < 10; i++) {
        data_reg2[i] -= data_reg2[i - 2];
    }
}

void ClassifierLayer::segmentPlot() {
    static ImPlotColormap Liars = -1;
    if (Liars == -1) {
        static const ImU32 Liars_Data[6] = { 4282515870, 4282609140, 4287357182, 4294630301, 4294945280, 4294921472 };
        Liars = ImPlot::AddColormap("Liars", Liars_Data, 5);
    }

    static const char* politicians[] = { "Input", "Expert" };
    std::array<int, 10> values = sharedData->alignedSegments;
    static int data_reg[10];
    PrepareData(values, data_reg);
    static const char* labels_reg[] = { "Squat preparation", "Squat going down", "Squat is down", "Squat going up", "Squat wrap up" };

    SetupPlot("Movement Segments aligned", politicians, data_reg, labels_reg, Liars);

    float progress = static_cast<float>(DR::getI()->getContext()->c_frame % std::get<1>(*DR::getI()->getContext()->matrix).size()) / std::get<1>(*DR::getI()->getContext()->matrix).size();
    ImGui::Dummy(ImVec2(50, 2));
    ImGui::SameLine();
    ImGui::ProgressBar(progress, ImVec2(1605, 2));
}

void ClassifierLayer::segmentPlotUnAligned() {
    static ImPlotColormap Liars2 = -1;
    if (Liars2 == -1) {
        static const ImU32 Liars_Data2[6] = { 4282515870, 4282609140, 4287357182, 4294630301, 4294945280, 4294921472 };
        Liars2 = ImPlot::AddColormap("Liars2", Liars_Data2, 5);
    }

    static const char* politicians2[] = { "Input", "Expert" };
    std::array<int, 5> in_values = sharedData->inp_segments;
    std::array<int, 5> ref_values = sharedData->ref_segments;
    static int data_reg2[10];
    PrepareUnalignedData(in_values, ref_values, data_reg2);
    static const char* labels_reg2[] = { "Squat preparation", "Squat going down", "Squat is down", "Squat going up", "Squat wrap up" };

    SetupPlot("Movement Segments not aligned", politicians2, data_reg2, labels_reg2, Liars2);
}