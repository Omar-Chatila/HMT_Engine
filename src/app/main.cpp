#include "engine/engine.h"
#include "motion_file_processor.h"
#include "../tests/tests.h"

int main(int argc, char *argv[]) {
    std::cout << "JAAAAAAAAAA" << std::endl;
    auto *sharedData = new SharedData();
    auto *motionFileProcessor = new MotionFileProcessor(SQUATS);
    const char *default_file = "fb_21_pre_splitted_5.txt";
    motionFileProcessor->processInputFile(std::string(default_file));
    auto kNNResults = motionFileProcessor->getKClosestMatches(16, DTW);
    TrajectoryAnalysisManager *manager = kNNResults.front();
    manager->updateDisplayRequirements();
    for (auto result: kNNResults) {
        TrajectoryInfo info;
        info.reference = result->getContext()->reference_file;
        info.manager = result;
        for (int i = 0; i < ALGO_COUNT; i++) {
            float cost = result->getAlgorithmResult(static_cast<Algorithm>(i));
            info.costs.push_back(cost);
        }
        sharedData->trajectoryInfos.push_back(info);
    }
    sharedData->alignedSegments = DR::getI()->getContext()->matching_algorithms[CDTW]->squat_segments;
    sharedData->wdtw_alignedSegments = DR::getI()->getContext()->matching_algorithms[WEIGHTDTW]->squat_segments;
    sharedData->wddtw_alignedSegments = DR::getI()->getContext()->matching_algorithms[WEIGHTDDTW]->squat_segments;
    sharedData->inp_segments = calculateSegments(DR::getI()->getInp_frames());
    sharedData->ref_segments = calculateSegments(DR::getI()->getRef_frames());
    sharedData->currentAnalysis = motionFileProcessor;
    auto *rend = new Renderer(sharedData);
    rend->display();

    delete rend;
    delete motionFileProcessor;
    delete sharedData;

    //test();
    //test2();
    return 0;
}
