#include "renderer.h"
#include "trajectory_analysis_manager.h"
#include "motion_file_processor.h"

int main() {
    /*
    UIContext *context = new UIContext();
    std::string input_file = R"(resources\motion_data\squats\fb_41_pre_splitted_1.txt)";
    std::string ref_file = R"(resources\motion_data\squats\expertise_01_single100_2_splitted_1.txt)";
    TrajectoryAnalysisManager *manager = new TrajectoryAnalysisManager(input_file, ref_file, context);
    manager->performAnalysis();
    manager->updateContext();
    */

    MotionFileProcessor* motionFileProcessor = new MotionFileProcessor(SQUATS);
    const char* file = "fb_01_pre_splitted_1.txt";
    motionFileProcessor->processInputFile(std::string(file));
    auto manager = motionFileProcessor->getClosestMatch(DTW);
    manager->displayRequirements().context->motion_files
    Renderer *renderer = new Renderer(manager->displayRequirements().context);
    renderer->display(manager);

    delete renderer;
    delete motionFileProcessor;

    return 0;
}
