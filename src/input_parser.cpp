#include "input_parser.h"

Input_parser::Input_parser(const char *path_to_file) {
    this->file = path_to_file;
}

Input_parser::~Input_parser() {
    for (Frame f : this->frames) {
        free(f.joint_rotations);
        free(f.joint_translations);
    }
}


std::vector<std::string> Input_parser::readAllLines() {
    std::ifstream file(this->file);
    std::vector<std::string> lines;
    std::string line;

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }
    int line_number = 1;
    while (std::getline(file, line)) {
        // filter meta info
        if (line_number++ > 15)
            lines.push_back(line);
    }
    file.close();
    return lines;
}


std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(str);

    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
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

    if (time_frame == 0) {
        std::cout << "root" << root_translation << std::endl;
    }

    if (time_frame == 0) {
        auto beg = parts.begin();
        std::cout << "timeframe" <<parts.begin()[0] << std::endl;
    }

    // delete timeframe part
    parts.erase(parts.begin());
    //delete root translation part

    if (time_frame == 0) {
        auto beg = parts.begin();
        std::cout << "root tr" << parts.begin()[0] << std::endl;
    }

    parts.erase(parts.begin());

    if (time_frame == 0) {
        auto beg = parts.begin();
        std::cout << "first joint" << parts.begin()[0] << std::endl;
    }

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
        if (time_frame == 0) {
            std::cout << i << "\n";
            std::cout << "x: " << std::stod(coords[0]) << "  ";
            std::cout << "y: " << std::stod(coords[1]) << "  ";
            std::cout << "z: " << std::stod(coords[2]) << std::endl;
        }
        float x2 = x + std::stod(coords[0]);
        float y2 = y + std::stod(coords[1]);
        float z2 = z + std::stod(coords[2]);
        joint_translations[joint_index] = Vec3D{x2, y2, z2};
    }

    // Extract and process meta
    std::string meta = parts_and_meta[1];
    std::vector<std::string> meta_parts = split(meta, ',');

    // Process movement_segment
    std::string movement_segment = meta_parts[0];
    std::size_t pos = movement_segment.find(":");      
    movement_segment = movement_segment.substr(pos + 1);

    // Process style_pattern
    std::string style_pattern = meta_parts[1];
    pos = style_pattern.find(":");   
    style_pattern = style_pattern.substr(pos + 1); // +1 to skip the colon
    std::vector<std::string> patterns = split(style_pattern, ' ');

    // Combine movement_segment and patterns into one vector
    std::vector<std::string> meta_data;
    meta_data.push_back(movement_segment);
    meta_data.insert(meta_data.end(), patterns.begin(), patterns.end());
    return {time_frame, root_translation, joint_rotations, joint_translations, meta_data};
}

std::vector<Frame> Input_parser::get_frames() {
    std::vector<std::string> lines = readAllLines();
    int time_frame = 0;
    for (auto& line : lines) {
        this->frames.push_back(line_to_frame(line, time_frame++));
    }
    return this->frames;
}