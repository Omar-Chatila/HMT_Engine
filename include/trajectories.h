#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include "util.h"
#include <array>

class Trajectories {
private:
    int length;
    std::array<Vec3D*, JOINT_COUNT> positions_per_joint;
    std::array<Quaternion*, JOINT_COUNT> angles_per_joint;
    Vec3D* root_trajectory;
    std::vector<Frame>& frames;

public:
    Trajectories(std::vector<Frame>& p_frames) : frames{p_frames}, length{static_cast<int>(p_frames.size())} {
        for (auto& joint_positions : positions_per_joint) {
            joint_positions = new Vec3D[length];
        }
        for (auto& joint_angles : angles_per_joint) {
            joint_angles = new Quaternion[length];
        }
        root_trajectory = new Vec3D[length];
        for (const auto joint : Joint::All) {
            set_trajectory(joint);
        }
    }

    ~Trajectories() {
        for (auto joint_positions : positions_per_joint) {
            delete[] joint_positions;
        }
        for (auto joint_angles : angles_per_joint) {
            delete[] joint_angles;
        }
    }

    Vec3D* get_rootTrajectory() {
        return this->root_trajectory;
    }

    Vec3D* get_positionsTrajectory(Joint::Type joint) {
        return positions_per_joint[joint];
    }
    
    Quaternion* get_anglesTrajectory(Joint::Type joint) {
        return angles_per_joint[joint];
    }

    int size() {
        return this->length;
    }

private:
    void set_rootTrajectory() {
        for (int current_frame = 0; current_frame < this->length; ++current_frame) {
            root_trajectory[current_frame] = this->frames[current_frame].root_translation;
        }
    }
    
    void set_trajectory(Joint::Type joint) {
        for (int current_frame = 0; current_frame < this->length; ++current_frame) {
            positions_per_joint[joint][current_frame] = this->frames[current_frame].joint_translations[joint];
            angles_per_joint[joint][current_frame] = this->frames[current_frame].joint_rotations[joint];
        }
    }
};

#endif
