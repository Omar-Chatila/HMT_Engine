#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include "util.h"
#include <vector>

class Trajectories {
private:
    int length;
    std::vector<Vec3D *> positions_per_joint;
    std::vector<Quaternion *> angles_per_joint;
    std::vector<Frame> &frames;

public:
    explicit Trajectories(std::vector<Frame> &p_frames) :
            frames{p_frames},
            length{static_cast<int>(p_frames.size())},
            positions_per_joint{p_frames.size(), nullptr},
            angles_per_joint{p_frames.size(), nullptr} {
        for (int i = 0; i < length; i++) {
            positions_per_joint[i] = p_frames[i].joint_translations;
            angles_per_joint[i] = p_frames[i].joint_rotations;
        }
    }

    ~Trajectories() = default;

    Vec3D *get_positionsTrajectory(Joint::Type joint) {
        auto *ppj = new Vec3D[length];
        for (int i = 0; i < length; i++) {
            ppj[i] = positions_per_joint[i][static_cast<int>(joint)];
        }
        return ppj;
    }

    Quaternion *get_anglesTrajectory(Joint::Type joint) {
        return angles_per_joint[joint];
    }

    [[nodiscard]] const std::vector<Quaternion *> &get_anglesTrajectories() const {
        return angles_per_joint;
    }

    [[nodiscard]] const std::vector<Vec3D *> &get_positions_trajectories() const {
        return positions_per_joint;
    }

    [[nodiscard]] int size() const {
        return this->length;
    }

    // Hier stattdessen durch größte norm teilen
    static std::vector<std::pair<Quaternion, Quaternion>> max_rotations(const std::vector<Quaternion *> &trajectory) {
        std::vector<Quaternion> max(JOINT_COUNT, Quaternion(-std::numeric_limits<float>::infinity(),
                                                            -std::numeric_limits<float>::infinity(),
                                                            -std::numeric_limits<float>::infinity(),
                                                            -std::numeric_limits<float>::infinity()));
        std::vector<Quaternion> min(JOINT_COUNT, Quaternion(std::numeric_limits<float>::infinity(),
                                                            std::numeric_limits<float>::infinity(),
                                                            std::numeric_limits<float>::infinity(),
                                                            std::numeric_limits<float>::infinity()));
        for (const auto &quat: trajectory) {
            for (int joint = 0; joint < JOINT_COUNT; ++joint) {
                max[joint].w = std::max(max[joint].w, quat[joint].w);
                max[joint].x = std::max(max[joint].x, quat[joint].x);
                max[joint].y = std::max(max[joint].y, quat[joint].y);
                max[joint].z = std::max(max[joint].z, quat[joint].z);

                min[joint].w = std::min(min[joint].w, quat[joint].w);
                min[joint].x = std::min(min[joint].x, quat[joint].x);
                min[joint].y = std::min(min[joint].y, quat[joint].y);
                min[joint].z = std::min(min[joint].z, quat[joint].z);
            }
        }

        std::vector<std::pair<Quaternion, Quaternion>> range(JOINT_COUNT);
        for (int joint = 0; joint < JOINT_COUNT; ++joint) {
            range[joint] = {min[joint], max[joint]};
        }
        return range;
    }
};

#endif
