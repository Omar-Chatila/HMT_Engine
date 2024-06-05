#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

const int JOINT_COUNT = 19;

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

enum Error {

};

struct Vec3D {
    float x, y, z;

    friend std::ostream& operator<<(std::ostream& os, const Vec3D& vec) {
        os << "V(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
        return os;
    }
};

struct Quaternion
{
    float w, x, y, z;

    friend std::ostream& operator<<(std::ostream& os, const Quaternion& quat) {
        os << "Q(" << quat.w << ", " << quat.x << ", " << quat.y << ", " << quat.z << ")";
        return os;
    }
};

inline float operator*(const Quaternion& q1, const Quaternion& q2) {
    return q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
}

struct Frame {
    int time_frame;
    Vec3D root_translation;
    Quaternion* joint_rotations = static_cast<Quaternion*>(malloc(JOINT_COUNT * sizeof(Quaternion)));
    Vec3D* joint_translations = static_cast<Vec3D*>(malloc(JOINT_COUNT * sizeof(Vec3D)));
    std::vector<std::string> labels;

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

        os << "Labels: ";
        for (const std::string& label : frame.labels) {
            os << label << " ";
        }
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