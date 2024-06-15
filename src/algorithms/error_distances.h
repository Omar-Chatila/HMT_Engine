#ifndef DISTANCEMEASURES_HMT_ERROR_DISTANCES_H
#define DISTANCEMEASURES_HMT_ERROR_DISTANCES_H

#include "util.h"

namespace Joint {
// Distance functions for error recognition
    inline float too_deep_dist(const Quaternion *first, const Quaternion *second) {
        float distance = 0.0f;
        distance += 1 - std::abs(first[l_hip] * second[l_hip]);
        distance += 1 - std::abs(first[l_knee] * second[l_knee]);
        distance += 1 - std::abs(first[l_ankle] * second[l_ankle]);

        distance += 1 - std::abs(first[r_hip] * second[r_hip]);
        distance += 1 - std::abs(first[r_knee] * second[r_knee]);
        distance += 1 - std::abs(first[r_ankle] * second[r_ankle]);
        return distance;
    }

    inline float feet_too_close_dist(const Quaternion *first, const Quaternion *second) {
        float distance = 1 - std::abs(first[l_ankle] * second[l_ankle]);
        distance += 1 - std::abs(first[r_ankle] * second[r_ankle]);
        return distance;
    }

    /* Center of mass and overall body balance
     * Lower spine, critical for balance
     * Mid-spine, also relevant for balance and posture
     * Upper spine, affects posture and balance
     * Left hip, important for lower body weight distribution
     * Left knee, crucial for assessing leg alignment
     * Left ankle, key for foot placement and balance
     * Right hip, important for lower body weight distribution
     * Right knee, crucial for assessing leg alignment
     * Right ankle, key for foot placement and balance */
    inline float incorrect_weight_dist(const Quaternion *first, const Quaternion *second) {
        float distance = 0.0f;
        distance += 1 - std::abs(first[HumanoidRoot] * second[HumanoidRoot]);
        distance += 1 - std::abs(first[vl5] * second[vl5]);
        distance += 1 - std::abs(first[vc7] * second[vc7]);
        distance += 1 - std::abs(first[l_hip] * second[l_hip]);
        distance += 1 - std::abs(first[l_knee] * second[l_knee]);
        distance += 1 - std::abs(first[l_ankle] * second[l_ankle]);
        distance += 1 - std::abs(first[r_hip] * second[r_hip]);
        distance += 1 - std::abs(first[r_knee] * second[r_knee]);
        distance += 1 - std::abs(first[r_ankle] * second[r_ankle]);
        return distance;
    }

    // maybe use movement segments instead
    inline float hips_dont_start_dist(const Quaternion *first, const Quaternion *second) {
        float distance = 0.0f;
        distance += 1 - std::abs(first[HumanoidRoot] * second[HumanoidRoot]);
        distance += 1 - std::abs(first[l_hip] * second[l_hip]);
        distance += 1 - std::abs(first[r_hip] * second[r_hip]);
        return distance;
    }

    inline float movement_dyn_dist(const Quaternion *first, const Quaternion *second) {
        float distance = 0.0f;
        distance += 1 - std::abs(first[l_hip] * second[l_hip]);
        distance += 1 - std::abs(first[l_knee] * second[l_knee]);
        distance += 1 - std::abs(first[l_ankle] * second[l_ankle]);

        distance += 1 - std::abs(first[r_hip] * second[r_hip]);
        distance += 1 - std::abs(first[r_knee] * second[r_knee]);
        distance += 1 - std::abs(first[r_ankle] * second[r_ankle]);
        return distance;
    }

    inline float arched_neck_dist(const Quaternion *first, const Quaternion *second) {
        float distance = 0.0f;
        distance += 1 - std::abs(first[skullbase] * second[skullbase]);
        distance += 1 - std::abs(first[vc7] * second[vc7]);
        return distance;
    }

    inline float hollow_back_dist(const Quaternion *first, const Quaternion *second) {
        float distance = 0.0f;
        distance += 1 - std::abs(first[skullbase] * second[skullbase]);
        distance += 1 - std::abs(first[vl5] * second[vl5]);
        distance += 1 - std::abs(first[vt10] * second[vt10]);
        distance += 1 - std::abs(first[vc7] * second[vc7]);
        distance += 1 - std::abs(first[HumanoidRoot] * second[HumanoidRoot]);
        return distance;
    }

    inline float knees_sideways_dist(const Quaternion *first, const Quaternion *second) {
        float distance = 0.0f;
        distance += 1 - std::abs(first[l_knee] * second[l_knee]);
        distance += 1 - std::abs(first[r_knee] * second[r_knee]);
        return distance;
    }

    inline float symmetry_dist(const Quaternion *first, const Quaternion *second) {
        float distance = 0.0f;
        distance += 1 - std::abs(first[l_hip] * second[l_hip]);
        distance += 1 - std::abs(first[l_knee] * second[l_knee]);
        distance += 1 - std::abs(first[l_ankle] * second[l_ankle]);
        distance += 1 - std::abs(first[l_shoulder] * second[l_shoulder]);

        distance += 1 - std::abs(first[r_hip] * second[r_hip]);
        distance += 1 - std::abs(first[r_knee] * second[r_knee]);
        distance += 1 - std::abs(first[r_ankle] * second[r_ankle]);
        distance += 1 - std::abs(first[r_shoulder] * second[r_shoulder]);
        return distance;
    }
}

#endif //DISTANCEMEASURES_HMT_ERROR_DISTANCES_H
