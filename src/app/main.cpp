#include "renderer.h"
#include "trajectory_analysis_manager.h"
#include "motion_file_processor.h"

int main() {
    MotionFileProcessor* motionFileProcessor = new MotionFileProcessor(SQUATS);
    const char* default_file = "fb_41_pre_splitted_1.txt";
    motionFileProcessor->processInputFile(std::string(default_file));
    auto manager = motionFileProcessor->getClosestMatch(DTW);

    auto current_context = manager->displayRequirements().context;
    const char* squats_info = R"(resources\squats_subject_info.csv)";
    current_context->motion_files = motion_info(squats_info);

    Renderer *renderer = new Renderer(current_context);
    renderer->display(manager);

    delete renderer;
    delete motionFileProcessor;

    return 0;
}
