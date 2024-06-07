#include "enums.h"

namespace Joint {
    std::vector<std::pair<int, int>> bones = {
            {l_shoulder, l_sternoclavicular},
            {l_sternoclavicular, r_sternoclavicular},
            {r_sternoclavicular, r_shoulder},

            {skullbase, vc7},
            {vc7, vt10},
            {vt10, vl5},

            {HumanoidRoot, l_hip},
            {HumanoidRoot, r_hip},
            {HumanoidRoot, vl5},

            {l_shoulder, l_elbow},
            {l_elbow, l_wrist},
            {r_shoulder, r_elbow},
            {r_elbow, r_wrist},

            {l_hip, l_knee},
            {l_knee, l_ankle},
            {r_hip, r_knee},
            {r_knee, r_ankle}
    };
}

std::vector<glm::vec3> sphereColors = {
        glm::vec3(1.0f, 0.5f, 1.0f),

        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),

        glm::vec3(0.0f, 1.0f, 0.0f),

        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),

        glm::vec3(0.0f, 1.0f, 0.0f),

        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),

        glm::vec3(1.0f, 1.0f, 0.0f),

        glm::vec3(1.0f, 0.8f, 0.4f),
        glm::vec3(0.4f, 0.2f, 0.8f),

        glm::vec3(1.0f, 1.0f, 0.0f),

        glm::vec3(1.0f, 0.8f, 0.4f),
        glm::vec3(0.4f, 0.2f, 0.8f)
};
