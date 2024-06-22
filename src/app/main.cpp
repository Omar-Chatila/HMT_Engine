#include "Engine.h"
#include "motion_file_processor.h"
#include "tests.h"

int main() {
    SharedData *sharedData = new SharedData();
    MotionFileProcessor* motionFileProcessor = new MotionFileProcessor(SQUATS);
    const char* default_file = "fb_21_pre_splitted_5.txt";
    motionFileProcessor->processInputFile(std::string(default_file));
    DR* disp_req = DR::getI();
    TrajectoryAnalysisManager *manager = motionFileProcessor->getClosestMatch(DTW);
    manager->updateDisplayRequirements();
    auto kNNResults = motionFileProcessor->getKClosestMatches(16, DTW);
    for (auto result : kNNResults) {
        TrajectoryInfo info;
        info.reference = result->getContext()->reference_file;
        info.manager = result;
        for (int i = 0; i < ALGO_COUNT; i++) {
            float cost = result->getAlgorithmResult(static_cast<Algorithm>(i));
            info.costs.push_back(cost);
        }
        sharedData->trajectoryInfos.push_back(info);
    }
    sharedData->alignedSegments = calcSegmentsAligned(std::get<1>(*DR::getI()->getContext()->matrix),DR::getI()->getInp_frames(),DR::getI()->getRef_frames());
    sharedData->inp_segments = calculateSegments(DR::getI()->getInp_frames());
    sharedData->ref_segments = calculateSegments(DR::getI()->getRef_frames());
    Renderer* rend = new Renderer(sharedData);
    rend->display();

    delete rend;
    delete motionFileProcessor;
    delete sharedData;
    //test();
    //test2();
    return 0;
}
