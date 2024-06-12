#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>

const int JOINT_COUNT = 19;
const int ERROR_COUNT = 9;

namespace Joint {
    enum Type {
        HumanoidRoot,
        vl5, vt10, vc7, skullbase, 
        l_sternoclavicular, l_shoulder, l_elbow, l_wrist, 
        r_sternoclavicular, r_shoulder, r_elbow, r_wrist, 
        l_hip, l_knee, l_ankle, 
        r_hip, r_knee, r_ankle
    };

    static const Type All[] = { HumanoidRoot, vl5, vt10, vc7, skullbase, 
        l_sternoclavicular, l_shoulder, l_elbow, l_wrist, 
        r_sternoclavicular, r_shoulder, r_elbow, r_wrist, 
        l_hip, l_knee, l_ankle, 
        r_hip, r_knee, r_ankle 
    };
}

struct Vec3D {
    float x, y, z;
    Vec3D() : x(0.0f), y(0.0f), z(0.0f) {}
    Vec3D(float p_x, float p_y, float p_z) : x(p_x), y(p_y), z(p_z) {}
    friend std::ostream& operator<<(std::ostream& os, const Vec3D& vec) {
        os << "V(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
        return os;
    }
};

struct Quaternion {
    float w, x, y, z;

    friend std::ostream& operator<<(std::ostream& os, const Quaternion& quat) {
        os << "Q(" << quat.w << ", " << quat.x << ", " << quat.y << ", " << quat.z << ")";
        return os;
    }
};

inline float operator*(const Quaternion& q1, const Quaternion& q2) {
    return q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
}

enum class MovementSegment {
    SQUAT_PREPARATION,
    SQUAT_GOING_DOWN,
    SQUAT_GOING_UP,
    SQUAT_WRAP_UP,
};

enum class ErrorPattern {
    TOO_DEEP,
    FEET_DISTANCE_NOT_SUFFICIENT,
    INCORRECT_WEIGHT_DISTRIBUTION,
    HIPS_DONT_START,
    WRONG_MOVEMENT_DYNAMICS,
    ARCHED_NECK,
    HOLLOW_BACK,
    KNEES_SIDEWAYS,
    SYMMETRY,
    COUNT
};

const std::unordered_map<MovementSegment, std::string> movementSegmentMap = {
        {MovementSegment::SQUAT_PREPARATION, "Squat-preparation"},
        {MovementSegment::SQUAT_GOING_DOWN, "Squat-going_down"},
        {MovementSegment::SQUAT_GOING_UP, "Squat-going_up"},
        {MovementSegment::SQUAT_WRAP_UP, "Squat-wrap_up"},
};

const std::unordered_map<ErrorPattern, std::string> errorPatternMap = {
        {ErrorPattern::TOO_DEEP, "too-deep"},
        {ErrorPattern::FEET_DISTANCE_NOT_SUFFICIENT, "feet-distance-not-sufficient"},
        {ErrorPattern::INCORRECT_WEIGHT_DISTRIBUTION, "incorrect-weight-distribution"},
        {ErrorPattern::HIPS_DONT_START, "hips-do-not-start"},
        {ErrorPattern::WRONG_MOVEMENT_DYNAMICS, "wrong-movement-dynamics"},
        {ErrorPattern::ARCHED_NECK, "arched_neck"},
        {ErrorPattern::HOLLOW_BACK, "hollow-back"},
        {ErrorPattern::KNEES_SIDEWAYS, "knees-sideways"},
        {ErrorPattern::SYMMETRY, "symmetry"},
};

struct Meta {
    MovementSegment segment;
    std::array<Vec3D, ERROR_COUNT> parameters;

    friend std::ostream& operator<<(std::ostream& os, const Meta& meta) {
        os << "Movement Segment: " << movementSegmentMap.at(meta.segment) << "\n";
        os << "Errors and Parameters:\n";
        
        for (int i = 0; i < ERROR_COUNT; ++i) {
            os << "  " << errorPatternMap.at(static_cast<ErrorPattern>(i)) << ": " << meta.parameters[i] << "\n";
        }
        
        return os;
    }
};

struct Frame {
    int time_frame;
    Vec3D root_translation;
    Quaternion* joint_rotations = static_cast<Quaternion*>(malloc(JOINT_COUNT * sizeof(Quaternion)));
    Vec3D* joint_translations = static_cast<Vec3D*>(malloc(JOINT_COUNT * sizeof(Vec3D)));
    Meta meta_info;

    friend std::ostream& operator<<(std::ostream& os, const Frame& frame) {
        os << "Frame " << frame.time_frame << ": Root translation: " << frame.root_translation << "\n";
        
        os << "Joint Rotations:\n";
        for (int i = 0; i < JOINT_COUNT; ++i) {
            os << "  " << frame.joint_rotations[i] << "\n";
        }
        
        os << "Joint Translations:\n";
        for (int i = 0; i < JOINT_COUNT; ++i) {
            os << "  " << frame.joint_translations[i] << "\n";
        }

        os << frame.meta_info;
        os << "\n";
        
        return os;
    }
};

inline std::string cropString(const std::string& input, char token) {
    size_t lastSlash = input.rfind(token);
    size_t lastDot = input.rfind('.');
    if (lastSlash == std::string::npos || lastDot == std::string::npos || lastSlash >= lastDot) {
        return input;  // Return the original string if conditions aren't met
    }
    return input.substr(lastSlash + 1, lastDot - lastSlash - 1);
}
inline std::string cropString(const std::string& input) {
    return cropString(input, '\\');
}


inline std::vector<std::string> readAllLines(const char* p_file, int start_line) {
    std::ifstream file(p_file);
    std::vector<std::string> lines;
    std::string line;

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }
    int line_number = 1;
    while (std::getline(file, line)) {
        // filter meta info
        if (line_number++ > start_line)
            lines.push_back(line);
    }
    file.close();
    return lines;
}

inline std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(str);

    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

#endif