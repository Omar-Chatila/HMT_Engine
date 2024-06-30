#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include "util.h"
#include <vector>

class Trajectories {
private:
    int length;
    std::vector<Vec3D*> positions_per_joint;
    std::vector<Quaternion*> angles_per_joint;
    std::vector<Frame>& frames;

public:
    Trajectories(std::vector<Frame>& p_frames) : 
        frames{p_frames}, 
        length{static_cast<int>(p_frames.size())},
        positions_per_joint{p_frames.size(), nullptr},
        angles_per_joint{p_frames.size(), nullptr}
    {
        for (int i = 0; i < length; i++) {
            positions_per_joint[i] = p_frames[i].joint_translations;
            angles_per_joint[i] = p_frames[i].joint_rotations;
        }
    }

    ~Trajectories() {}

    Vec3D* get_positionsTrajectory(Joint::Type joint) {
        Vec3D* ppj = new Vec3D[length];
        for (int i = 0; i < length; i++) {
            ppj[i] = positions_per_joint[i][static_cast<int>(joint)];
        }
        return ppj;
    }
    
    Quaternion* get_anglesTrajectory(Joint::Type joint) {
        return angles_per_joint[joint];
    }

    const std::vector<Quaternion*>& get_anglesTrajectories() const {
        return angles_per_joint;
    }

    const std::vector<Vec3D*>& get_positions_trajectories() const {
        return positions_per_joint;
    }

    int size() {
        return this->length;
    }

private:
    void set_VecTrajectory_perJoint(Joint::Type joint) {
        for (int current_frame = 0; current_frame < this->length; ++current_frame) {
            positions_per_joint[joint][current_frame] = this->frames[current_frame].joint_translations[joint];
        }
    }

    void set_quat_trajectories() {
        for (int current_frame = 0; current_frame < this->length; ++current_frame) {
            angles_per_joint[current_frame] = this->frames[current_frame].joint_rotations;
        }
    }
};

#endif
