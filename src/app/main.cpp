#include "Engine.h"
#include "motion_file_processor.h"

int main() {
    MotionFileProcessor* motionFileProcessor = new MotionFileProcessor(SQUATS);
    const char* default_file = "fb_41_pre_splitted_1.txt";
    motionFileProcessor->processInputFile(std::string(default_file));
    TrajectoryAnalysisManager *manager = motionFileProcessor->getClosestMatch(DTW);
    Renderer* rend = new Renderer(manager);
    rend->display();

    delete motionFileProcessor;

    return 0;
}
