#include "renderer.h"
#include "trajectory_analysis_manager.h"

int main() {
    UIContext *context = new UIContext();
    std::string input_file = R"(resources\motion_data\squats\fb_41_pre_splitted_1.txt)";
    std::string ref_file = R"(resources\motion_data\squats\expertise_01_single100_2_splitted_1.txt)";
    TrajectoryAnalysisManager *manager = new TrajectoryAnalysisManager(input_file, ref_file, context);
    manager->performAnalysis();
    manager->updateContext();

    Renderer *renderer = new Renderer(context);
    int code = renderer->display(manager);

    delete renderer;
    delete manager;
    return code;
}
