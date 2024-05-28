#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

const int JOINT_COUNT = 18;

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

#endif