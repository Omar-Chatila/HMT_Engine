#include "engine/engine.h"
#include "motion_file_processor.h"
#include "data.h"
#include "../tests/tests.h"
#include <chrono>

Data *Data::instance = nullptr;

int main(int argc, char *argv[]) {
    // start time measurement
    std::chrono::time_point<std::chrono::system_clock> cpu_start = std::chrono::system_clock::now();
    std::cout << "Process all files..." << std::endl;
    // Parse all files
    auto *motionFileProcessor = new MotionFileProcessor(SQUATS);
    motionFileProcessor->processAllFiles();
    std::chrono::time_point<std::chrono::system_clock> cpu_end = std::chrono::system_clock::now();
    auto cpu_total_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(cpu_end - cpu_start).count();
    std::cout << cpu_total_time_ms << " ms" << std::endl;
    // Create objects for algorithms executions
    motionFileProcessor->createTrajectoryAnalysisManagers();
    std::cout << "All files processed." << std::endl;

    // Load first file
    const char *default_file = "fb_21_pre_splitted_5.txt";
    std::cout << "Load default file: " << default_file << std::endl;
    motionFileProcessor->processInputFile(std::string(default_file));
    std::cout << "find closest matches" << std::endl;

    // Find best match based on DTW
    auto kNNResults = motionFileProcessor->getKClosestMatches(16, std::string(default_file), DTW);
    TrajectoryAnalysisManager *manager = kNNResults.front();
    manager->setSegmentsAndMatchings();
    Data *data = Data::getInstance(motionFileProcessor, manager);

    // Start engine
    std::cout << "Start GUI" << std::endl;
    PLAY = true;
    auto *rend = new Renderer(data);
    rend->display();

    // Let the OS free most the memory...
    Data::free();
    delete rend;
    return 0;
}
