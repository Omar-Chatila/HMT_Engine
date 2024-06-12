#include "input_parser.h"

Input_parser::Input_parser(const char *path_to_file) : min_x(0.0f), max_x(0.0f), min_y(0.0f), max_y(0.0f), min_z(0.0f), max_z(0.0f) {
    this->file = path_to_file;
}

Input_parser::~Input_parser() {
    for (Frame f : this->frames) {
        free(f.joint_rotations);
        free(f.joint_translations);
    }
}

Frame Input_parser::line_to_frame(std::string &line, int time_frame) {
    // separate movement data and meta info
    std::vector<std::string> parts_and_meta = split(line, ';');
    std::vector<std::string> parts = split(parts_and_meta[0], ',');

    auto root_string = split(parts[1], ' ');

    // Extract root translation
    const float x = std::stod(root_string[0]);
    const float y = std::stod(root_string[1]);
    const float z = std::stod(root_string[2]);
    Vec3D root_translation{x, y, z};

    // delete timeframe part
    parts.erase(parts.begin());
    //delete root translation part
    parts.erase(parts.begin());

    int joint_index = 0;
    Quaternion* joint_rotations = (Quaternion *) (malloc(JOINT_COUNT * sizeof(Quaternion)));
    Vec3D* joint_translations = (Vec3D *) (malloc(JOINT_COUNT * sizeof(Vec3D)));

    // Extract joint rotations
    for (; joint_index < JOINT_COUNT; joint_index++) {
        std::string component = parts[joint_index];
        std::vector<std::string> coords = split(component, ' ');
        float w = std::stod(coords[0]);
        float x1 = std::stod(coords[1]);
        float y1 = std::stod(coords[2]);
        float z1 = std::stod(coords[3]);
        joint_rotations[joint_index] = Quaternion{w, x1, y1, z1};
    }

    // Extract joint translations 
    joint_index = 0;
    for (int i = JOINT_COUNT; i < 2 * JOINT_COUNT; i++, joint_index++) {
        std::string component = parts[i];
        std::vector<std::string> coords = split(component, ' ');
        float x2 = x + std::stod(coords[0]);
        float y2 = y + std::stod(coords[1]);
        float z2 = z + std::stod(coords[2]);
        joint_translations[joint_index] = Vec3D{x2, y2, z2};

        // Update min and max values
        if (x2 < min_x) min_x = x2;
        if (x2 > max_x) max_x = x2;
        if (y2 < min_y) min_y = y2;
        if (y2 > max_y) max_y = y2;
        if (z2 < min_z) min_z = z2;
        if (z2 > max_z) max_z = z2;
    }

    std::vector<std::string> meta_data;
    // MovementSegment
    std::string metaData = parts_and_meta[1];
    MovementSegment currentSegment;
    std::vector<std::string> labels = split(metaData, ',');
    std::string segment = labels[0];

    for (const auto& [segIndex, segType] : movementSegmentMap) {
        if (segment.find(segType) != std::string::npos) {
            currentSegment = segIndex;
            break;
        }
    }
 
    std::array<Vec3D, ERROR_COUNT> parameters;

    // PSPS
    std::string errors = labels[1];
    // Single errors~2~2~2
    std::vector<std::string> errors_n_confidences = split(errors, ' ');
    std::string not_active = "not-active";
    
    for (auto& single_error : errors_n_confidences) {
        for (const auto& [errPattern, errType] : errorPatternMap) {
            int errIndex = static_cast<int>(errPattern);
            bool found = single_error.find(errType) != std::string::npos;
            if (found && single_error.find(not_active) == std::string::npos) {
                std::vector<std::string> root_string = split(single_error, '~');
                const float confidence = std::stof(root_string[1]);
                const float intensity = std::stof(root_string[2]);
                const float intensityConfidence = std::stof(root_string[3]);
                parameters[errIndex] = Vec3D{confidence, intensity, intensityConfidence};
                break;
            }
        }     
    }
    return {time_frame, root_translation, joint_rotations, joint_translations, {currentSegment, parameters}};
}

std::vector<Frame> Input_parser::get_frames() {
    std::vector<std::string> lines = readAllLines(this->file, 15);
    int time_frame = 0;
    for (auto& line : lines) {
        this->frames.push_back(line_to_frame(line, time_frame++));
    }
    return this->frames;
}
