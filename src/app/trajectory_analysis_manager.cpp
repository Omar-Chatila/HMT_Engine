/* #include "trajectory_analysis_manager.h"

TrajectoryAnalysisManager::TrajectoryAnalysisManager(const std::string& inputFile, const std::string& refFile, UIContext* context)
    : inputFile(inputFile), refFile(refFile), context(context), inputParser(nullptr), refParser(nullptr), inputTrajectories(nullptr), refTrajectories(nullptr), analysis(nullptr) {
    inputParser = new Input_parser(inputFile.c_str());
    inputFrames = inputParser->get_frames();
    inputTrajectories = new Trajectories(inputFrames);

    refParser = new Input_parser(refFile.c_str());
    refFrames = refParser->get_frames();
    refTrajectories = new Trajectories(refFrames);

    analysis = new Trajectoy_analysis(*inputTrajectories, *refTrajectories);
}

TrajectoryAnalysisManager::~TrajectoryAnalysisManager() {
    delete analysis;
    delete refTrajectories;
    delete refParser;
    delete inputTrajectories;
    delete inputParser;
}

void TrajectoryAnalysisManager::performAnalysis() {
    alignment = analysis->perform_DTW(inputTrajectories->get_anglesTrajectories(), refTrajectories->get_anglesTrajectories());
    context->cost = std::get<0>(alignment);
    analysis->perform_EDR(Joint::l_hip, EUCLID, 3.0);
}

void TrajectoryAnalysisManager::updateContext() {
    context->reference_file = cropString(refFile).c_str();
    context->input_file = cropString(inputFile).c_str();
    int n = inputTrajectories->get_anglesTrajectories().size();
    int m = refTrajectories->get_anglesTrajectories().size();
    float* matrixx = std::get<2>(alignment);
    std::tuple mat{matrixx, std::get<1>(alignment), n, m};
    context->matrix = &mat;
    const char* squats_info = R"(resources\squats_subject_info.csv)";
    std::vector<motion_data> info = motion_info(squats_info);
    context->motion_files = &info;
}

void TrajectoryAnalysisManager::render(Renderer& renderer, const glm::mat4& projection, const glm::mat4& view, Sphere& sphere, Shader& sphereShader) {
     if (display(refFrames, inputFrames, context, alignment)) {
        std::cout << "Render Error" << std::endl;
    } 
}
 */