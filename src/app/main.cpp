#include "Engine.h"
#include "motion_file_processor.h"

int main() {
    MotionFileProcessor* motionFileProcessor = new MotionFileProcessor(SQUATS);
    const char* default_file = "fb_41_pre_splitted_1.txt";
    motionFileProcessor->processInputFile(std::string(default_file));
    DR* disp_req = DR::getI();
    TrajectoryAnalysisManager *manager = motionFileProcessor->getClosestMatch(DTW);
    manager->updateDisplayRequirements();
    Renderer* rend = new Renderer();
    rend->display();

    delete rend;
    delete motionFileProcessor;

    return 0;
}
